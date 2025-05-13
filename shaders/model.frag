#version 460 core

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D specular;
layout(binding = 3) uniform sampler2D normal;
layout(binding = 4) uniform sampler2D emission;
layout(binding = 5) uniform sampler2D shadowsTexture;

// Lights grid for forward+ render
layout(rg32ui, binding = 6) uniform uimage2D lightsGrid;

struct ObjectTextures
{
    vec3 diffuseTex;
    vec3 specularTex;
    vec3 emissionTex;
    vec3 normalTex;
};


uniform bool normalMapping;


const uint MAX_CONCURRENT_SHADOWS_SOURCES = 4;
in VERTEX_OUT
{
    vec2 texCoord;
    vec3 tangentFragPos;
    mat3 TBN;
    vec3 normals;

    vec3 viewFragPos;

    vec3 lightsShadowsData[MAX_CONCURRENT_SHADOWS_SOURCES];
    vec3 lightViewFragPos[MAX_CONCURRENT_SHADOWS_SOURCES];
    uint lightForShadowsPresence[MAX_CONCURRENT_SHADOWS_SOURCES];
} fragment_in;


const int LIGHT_DIRECTIONAL = 0;
const int LIGHT_POINT = 1;
struct LightDesc
{
    vec3 data;
    vec3 color;
    float radius;
    int type;
    bool affectsOnShadows;
};

// DATA SHOULD BE IN VIEW SPACE
layout(std430, binding = 7) buffer lightsBuffer
{
    LightDesc lights[];
};

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

float AttenuatePointLight(vec3 viewLightPos, vec3 viewFragPos, float radius)
{
    const float distance = length(viewLightPos - viewFragPos);
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

vec3 CalculatePointLight(ObjectTextures textures)
{
    const float lightColorAmbient = 0.22;

    vec3 lightDir = normalize(fragment_in.viewFragPos - fragment_in.viewLightData);
    vec3 viewDirection = normalize(-fragment_in.viewFragPos);

    if(normalMapping)
    {
        // Convert to tangent space
        lightDir = fragment_in.TBN * lightDir;
        viewDirection = fragment_in.TBN * viewDirection;
    }
    const float dotProduct = dot(textures.normalTex, lightDir);
    float diffuseLight = max(dotProduct, 0.0);
    vec3 diffuseVec = diffuseLight * lightDescriptor.color;


    const vec3 halfwayDirection = normalize(lightDir + viewDirection);
    const float specularShininess = 32;
    const float specularLight = pow(max(dot(textures.normalTex, halfwayDirection), 0.0), specularShininess);
    vec3 specularVec = specularLight * textures.specularTex * lightDescriptor.color;

    const vec3 ambientVec = lightColorAmbient * textures.diffuseTex;

    const vec3 emissionVec = textures.emissionTex;

    float attenuation = AttenuatePointLight(fragment_in.viewLightData, fragment_in.viewFragPos, lightDescriptor.radius);

    diffuseVec  *= attenuation;
    specularVec *= attenuation;

    return ambientVec + diffuseVec + specularVec + emissionVec;
}

vec3 CalculateDirectionalLight(ObjectTextures textures)
{
    const float lightColorAmbient = 0.32;
    vec3 lightDir = normalize(fragment_in.viewLightData);
    vec3 viewDirection = normalize(fragment_in.viewFragPos);
    if(normalMapping)
    {
        // Convert to tangent space
        lightDir = fragment_in.TBN * lightDir;
        viewDirection = fragment_in.TBN * viewDirection;
    }
    // diffuse
    const float dotProduct = dot(textures.normalTex, lightDir);
    const float diffuseLightPower = 1.75;
    float diffuseLight = max(dotProduct, 0.0);
    vec3  diffuseVec = diffuseLight * textures.diffuseTex * diffuseLightPower * lightDescriptor.color;

    // specular
    // blinn phong model with halfway vector is far more useful than phong model as halfway never exceeds angle 90
    // except cases when light dir under the ground
    const vec3 halfwayDirection = normalize(lightDir + viewDirection);
    const float specularShininess = 32;
    const float specularLight = pow(max(dot(textures.normalTex, halfwayDirection), 0.0), specularShininess);
    vec3 specularVec = specularLight * textures.specularTex * lightDescriptor.color;

    // ambient
    const vec3 ambientVec = lightColorAmbient * textures.diffuseTex;
    // emission
    const vec3 emissionVec = textures.emissionTex;

    return ambientVec + diffuseVec + specularVec + emissionVec;
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

    textures.normalTex = fragment_in.normals;
    if(normalMapping == true)
    {
        textures.normalTex = texture(normal, fragment_in.texCoord).rgb;
        // converting from [0,1] range to [-1,1], otherwise normals would look only 1 side
        textures.normalTex = textures.normalTex * 2.0 - 1.0;
    }


    vec3 colors = vec3(0.0);
    float shadow = 0.0;
    switch(lightDescriptor.type)
    {
    case LIGHT_DIRECTIONAL:
        colors = CalculateDirectionalLight(textures);

        if(lightDescriptor.affectsOnShadows)
        {
            for(uint i = 0; i < MAX_CONCURRENT_SHADOWS_SOURCES; ++i)
            {
                if(fragment_in.lightForShadowsPresence[i] > 0)
                    shadow += CalculateShadows(fragment_in.lightViewFragPos[i], textures.normalTex, fragment_in.lightsShadowsData[i]);
            }
        }

        break;
    case LIGHT_POINT:
        colors = CalculatePointLight(textures);

        if(lightDescriptor.affectsOnShadows)
        {
            const vec3 lightViewDir = fragment_in.lightViewFragPos - fragment_in.lightViewLightData;
            for(uint i = 0; i < MAX_CONCURRENT_SHADOWS_SOURCES; ++i)
            {
                if(fragment_in.lightForShadowsPresence[i] > 0)
                    shadow += CalculateShadows(fragment_in.lightViewFragPos[i], textures.normalTex, fragment_in.lightsShadowsData[i]);
            }
        }
        break;

    default:
        return colors;
    }


    vec3 res = (1.0 - shadow) * colors;
    return res;
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