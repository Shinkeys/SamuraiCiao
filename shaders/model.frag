#version 460 core

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D specular;
layout(binding = 3) uniform sampler2D normal;
layout(binding = 4) uniform sampler2D emission;
// layout(binding = 5) uniform sampler2DArray shadowsTexturesArray;
layout(binding = 5) uniform sampler2D shadowsTexture;

// Lights grid for forward+ render
layout(rg32ui, binding = 6) uniform uimage2D lightsGrid;

const int PIXELS_PER_TILE = 16;


struct ObjectTextures
{
    vec3 diffuseTex;
    vec3 specularTex;
    vec3 emissionTex;
    vec3 normalTex;
};

const int LIGHT_TYPE_DIRECTIONAL = 0;
const int LIGHT_TYPE_POINT = 1;

const uint MAX_CONCURRENT_SHADOWS_SOURCES = 4;
in VERTEX_OUT
{
    vec2 texCoord;
    mat3 TBN;

    vec3 worldFragPos;
    vec3 backupNormals;

// for shadows
    vec3 lightsShadowsData[MAX_CONCURRENT_SHADOWS_SOURCES];
    vec3 lightViewFragPos[MAX_CONCURRENT_SHADOWS_SOURCES];
    flat int  lightShadowsType[MAX_CONCURRENT_SHADOWS_SOURCES];

    vec3 dirLightShadowsData;
    vec3 dirLightFragPos;
} fragment_in;


struct LightDesc
{
    vec4 data;
    vec4 color;
    float radius;
    int type;

    // If light is used for a shadow produce
    int shadowsDataIndex;

    int padding;
};

// ONLY POINT AND SPOT(TO DO) LIGHTS
layout(std430, binding = 7) readonly buffer lightsBuffer
{
    LightDesc lights[];
};

layout(std430, binding = 8) buffer lightsGlobalIndBuffer
{
    int lightsIndexList[];
};

struct LightCalcResult
{
    vec3 ambient;
    vec3 diffuse;
    vec3 emission;
    vec3 specular;
};

uniform vec3 directionalLightDir;
uniform vec3 cameraPosition;
uniform bool normalMapping;
uniform ivec2 screenDimension;
uniform bool drawShadows;

float CalculateShadows(vec3 lightViewFragPos, vec3 normals, vec3 lightViewLightDir)
{
    vec3 perspDivide = lightViewFragPos.xyz;

    vec3 ndcCoords = perspDivide * 0.5 + 0.5;

    float closestDepth = texture(shadowsTexture, ndcCoords.xy).r;

    float currentDepth = ndcCoords.z;

    if(ndcCoords.z > 1.0)
        return 0.0;

    float bias = max(0.00035 * (1.0 - dot(normalize(normals), lightViewLightDir)), 0.0001);

    // pcf. getting size of each pixel(texel) and then sampling values
    // from 9 surrounding texels. dividing it only by 9 to average result
    const vec2 texelSize = 1.0 / textureSize(shadowsTexture, 0);

    const float xOffset = texelSize.x;
    const float yOffset = texelSize.y;

    float shadow = 0.0;
    for(int y = -1; y <= 1; ++y)
    {
        for(int x = -1; x <= 1; ++x)
        {
            vec2 offsets = vec2(x * xOffset, y * yOffset);
            float pcf = texture(shadowsTexture, (ndcCoords.xy + offsets)).r;
            shadow += currentDepth - bias > pcf ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;


    return shadow;
}

float Square(float x)
{
    return x * x;
}

float AttenuatePointLight(vec3 lightPos, vec3 fragPos, float radius)
{
    const float distance = length(lightPos - fragPos);
    const float decaySpeed = 1.0;
    const float maxIntensity = 2.5; // basically represents 'start' point of the light brightness
    const float s = distance / radius;
    const float sqrS = Square(s);
    // check if distance < radius, otherwise would get wrong lightness
    // values at larger distances
    if(s >= 1.0)
        return 0.0;

    return maxIntensity * Square(1 - sqrS) / (1 + decaySpeed * sqrS);
}

LightCalcResult CalculatePointLight(ObjectTextures textures, LightDesc lightDesc)
{
    const float lightColorAmbient = 0.22;

    vec3 lightPos = lightDesc.data.xyz;
    vec3 lightDir = normalize(-(fragment_in.worldFragPos - lightPos));
    vec3 viewDirection = normalize(cameraPosition - fragment_in.worldFragPos);

    const float dotProduct = dot(textures.normalTex, lightDir);
    float diffuseLight = max(dotProduct, 0.0);
    vec3 diffuseVec = diffuseLight * lightDesc.color.xyz;


    const vec3 halfwayDirection = normalize(lightDir + viewDirection);
    const float specularShininess = 32;
    const float specularLight = pow(max(dot(textures.normalTex, halfwayDirection), 0.0), specularShininess);
    vec3 specularVec = specularLight * textures.specularTex * lightDesc.color.xyz;

    const vec3 ambientVec = lightColorAmbient * textures.diffuseTex;

    const vec3 emissionVec = textures.emissionTex;

//    // TO CHECK ATTENUATION!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    float attenuation = AttenuatePointLight(lightDesc.data.xyz, fragment_in.worldFragPos, lightDesc.radius);
    
    diffuseVec  *= attenuation;
    specularVec *= attenuation;

    LightCalcResult result;
    result.ambient = ambientVec;
    result.specular = specularVec;
    result.diffuse = diffuseVec;
    result.emission = emissionVec;
    return result;


}

LightCalcResult CalculateDirectionalLight(ObjectTextures textures)
{
    const float lightColorAmbient = 0.12;
    // Convert to tangent space
    vec3 lightDir = normalize(directionalLightDir);
    vec3 viewDirection = normalize(cameraPosition - fragment_in.worldFragPos);

    // diffuse
    const float dotProduct = dot(textures.normalTex, lightDir);
    const float diffuseLightPower = 1.25;
    float diffuseLight = max(dotProduct, 0.0);
    const vec3 dirLightColor = vec3(0.5, 0.5, 0.5);
    vec3  diffuseVec = diffuseLight * textures.diffuseTex * diffuseLightPower * dirLightColor;

    // specular
    // blinn phong model with halfway vector is far more useful than phong model as halfway never exceeds angle 90
    // except cases when light dir under the ground
    const vec3 halfwayDirection = normalize(lightDir + viewDirection);
    const float specularShininess = 32;
    const float specularLight = pow(max(dot(textures.normalTex, halfwayDirection), 0.0), specularShininess);
    vec3 specularVec = specularLight * textures.specularTex * dirLightColor;

    // ambient
    const vec3 ambientVec = lightColorAmbient * textures.diffuseTex;
    // emission
    const vec3 emissionVec = textures.emissionTex;

    LightCalcResult result;
    result.ambient = ambientVec;
    result.specular = specularVec;
    result.diffuse = diffuseVec;
    result.emission = emissionVec;
    return result;
}

vec3 CalculateLighting()
{
    ObjectTextures textures;
    // textures
    textures.diffuseTex = vec3(1.0, 1.0, 1.0);
    textures.diffuseTex = texture(diffuse, fragment_in.texCoord).rgb;

    textures.specularTex = vec3(0.0, 0.0, 0.0);
    textures.specularTex = texture(specular, fragment_in.texCoord).rgb;

    textures.emissionTex = vec3(1.0, 1.0, 0.0);
    textures.emissionTex = texture(emission, fragment_in.texCoord).rgb;

    if(normalMapping)
    {
        textures.normalTex = texture(normal, fragment_in.texCoord).rgb;
        // converting from [0,1] range to [-1,1], otherwise normals would look only 1 side
        textures.normalTex = textures.normalTex * 2.0 - 1.0;
        textures.normalTex = normalize(fragment_in.TBN * textures.normalTex);
    }
    else
    {
        textures.normalTex = fragment_in.backupNormals;
    }

    // Selecting index of current pixel to proceed lights
    ivec2 location = ivec2(gl_FragCoord.x, gl_FragCoord.y) / PIXELS_PER_TILE;
    // ivec2 tileIndex = ivec2(floor(location) / PIXELS_PER_TILE); // To reduce noiseness floor is useful

    uvec2 tileData = imageLoad(lightsGrid, location).xy;

    LightCalcResult totalColorResult;
    totalColorResult.ambient  = vec3(0.0, 0.0, 0.0);
    totalColorResult.diffuse  = vec3(0.0, 0.0, 0.0);
    totalColorResult.specular = vec3(0.0, 0.0, 0.0);
    totalColorResult.emission = vec3(0.0, 0.0, 0.0);
    float shadow = 0.0;
    if(!drawShadows)
    {
        for(uint i = 0; i < tileData.y; ++i)
        {
            uint lightIndex = lightsIndexList[i + tileData.x];
            LightDesc light = lights[lightIndex];

            LightCalcResult currentItColorRes;
            switch(light.type)
            {

            case LIGHT_TYPE_POINT:
                currentItColorRes = CalculatePointLight(textures, light);
                break;

            // TO DO: SPOT LIGHT IF WOULD NEED
            default:
                break;
            }
            totalColorResult.diffuse  += currentItColorRes.diffuse;
            totalColorResult.specular += currentItColorRes.specular;
        }

        vec3 finalColor = vec3(0.0);
        // Need to calculate ambient and emission only once.
        const float lightColorAmbient = 0.15;
        const vec3 ambientVec  = lightColorAmbient * textures.diffuseTex;
        const vec3 emissionVec = textures.emissionTex;

        finalColor = ambientVec + totalColorResult.diffuse.xyz
        + totalColorResult.specular.xyz + emissionVec;

        return finalColor;
    }

    vec3 shadowedLight = vec3(0.0);
    
    LightCalcResult directionalLightRes;
      directionalLightRes = CalculateDirectionalLight(textures);
    
    shadow = CalculateShadows(fragment_in.dirLightFragPos,
        textures.normalTex, fragment_in.dirLightShadowsData);
    shadowedLight = (1.0 - shadow) * (directionalLightRes.diffuse + directionalLightRes.specular);
    return shadowedLight;
    
}



out vec4 FragColor;
void main()
{
    vec3 result = CalculateLighting();
    FragColor = vec4(result, 1.0);
    // gamma correction
    const float gamma = 2.2;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
    // to do: gamma as post processing
}