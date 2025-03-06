#version 460 core


in vec2 TexCoord;

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D specular;
layout(binding = 3) uniform sampler2D normal;
layout(binding = 4) uniform sampler2D emission;
uniform bool normalMapping;

in vec3 viewfragPos;
in vec3 viewlightDir;
in vec3 lightViewLightDir;

in vec3 lightViewFragPos;


in vec3 normals;

uniform sampler2D shadowsTexture;
float CalculateShadows()
{
    vec3 perspDivide = lightViewFragPos.xyz;

    vec3 ndcCoords = perspDivide * 0.5 + 0.5;

    float closestDepth = texture(shadowsTexture, ndcCoords.xy).r;

    float currentDepth = ndcCoords.z;

    if(ndcCoords.z > 1.0)
        return 0.0;

    float bias = max(0.05 * (1.0 - dot(normalize(normals), lightViewLightDir)), 0.005);



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
            shadow += currentDepth > pcf ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;


    return shadow;
}


vec3 CalculateLighting()
{
    // textures
    vec3 diffuseTex = vec3(1.0, 1.0, 1.0);
    diffuseTex = texture(diffuse, TexCoord).rgb;
    
    vec3 specularTex = vec3(0.0, 0.0, 0.0);
    specularTex = texture(specular, TexCoord).rgb;

    vec3 emissionTex = vec3(1.0, 1.0, 0.0);
    emissionTex = texture(emission, TexCoord).rgb;

    vec3 normalMap = normals;
    if(normalMapping == true)
    {
        normalMap = texture(normal, TexCoord).rgb;
        // converting from [0,1] range to [-1,1], otherwise normals would look only 1 side
        normalMap = normalMap * 2.0 - 1.0;
    }

    // lights
    const float lightColorAmbient = 0.25;
    const vec3 lightColorDiffuse = vec3(0.5, 0.5, 0.5);
    const vec3 lightColorSpecular = vec3(1.0, 1.0, 1.0);


    const vec3 lightDirection = normalize(-viewlightDir);

    // diffuse
    const float dotProduct = dot(normalMap, lightDirection);
    const float diffuseLightPower = 5.0;
    float diffuseLight = max(dotProduct, 0.0);
    vec3 diffuseVec = (diffuseLight * diffuseTex * diffuseLightPower) * lightColorDiffuse;

    // specular
    const vec3 viewDirection = normalize(-viewfragPos);
    // blinn phong model with halfway vector is far more useful than phong model as halfway never exceeds angle 90
    // except cases when light dir under the ground
    const vec3 halfwayDirection = normalize(-lightDirection + viewDirection);
    const float specularShininess = 32;
    const float specularLight = pow(max(dot(viewDirection, halfwayDirection), 0.0), specularShininess);
    vec3 specularVec = (specularLight * specularTex) * lightColorSpecular;

    // ambient
    const vec3 ambientVec = lightColorAmbient * diffuseTex;

    // attenuation means that light attenuate depends on distance
    const vec3 imaginarySunPos = viewfragPos - vec3(0.0f, 2.5f, -2.0f);
    const float distance = length(imaginarySunPos - viewfragPos);
    const float attenuation = 1.0 / (distance);

    diffuseVec *= attenuation;
    specularVec *= attenuation;


    const float shadow = CalculateShadows();

    vec3 res = ((ambientVec) * (1.0 - shadow)) + (diffuseVec + specularVec + emissionTex);
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