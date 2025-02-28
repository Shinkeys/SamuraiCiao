#version 460 core


in vec2 TexCoord;

layout(binding = 1) uniform sampler2D diffuse;
layout(binding = 2) uniform sampler2D specular;
layout(binding = 3) uniform sampler2D normal;
layout(binding = 4) uniform sampler2D emission;

in vec3 viewfragPos;
in vec3 viewlightDir;
in vec3 normals;

in vec3 lightViewFragPos;



uniform sampler2D shadowsTexture;
float CalculateShadows()
{
    vec3 perspDivide = lightViewFragPos.xyz / lightViewFragPos.z;

    vec3 ndcCoords = perspDivide * 0.5 + 0.5;

    float closestDepth = texture(shadowsTexture, ndcCoords.xy).r;

    float currentDepth = ndcCoords.z;


    float bias = max(0.05 * (1.0 - dot(normals, viewlightDir)), 0.005);

    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
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

    // lights
    const float lightColorAmbient = 0.25;
    const vec3 lightColorDiffuse = vec3(0.5, 0.5, 0.5);
    const vec3 lightColorSpecular = vec3(1.0, 1.0, 1.0);


    const vec3 lightDirection = normalize(viewlightDir);

    // diffuse
    const float dotProduct = dot(normals, lightDirection);
    const float diffuseLightPower = 5.0;
    float diffuseLight = max(dotProduct, 0.0);
    vec3 diffuseVec = (diffuseLight * diffuseTex) * lightColorDiffuse;

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
    // const float distance = length(viewlightPos - viewfragPos);
    // const float attenuation = 1.0 / (distance);

    // diffuseVec *= attenuation;
    // specularVec *= attenuation;


    const float shadow = CalculateShadows();

    vec3 res = (ambientVec + ((diffuseVec + specularVec) * (1.0 - shadow)) + emissionTex);
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