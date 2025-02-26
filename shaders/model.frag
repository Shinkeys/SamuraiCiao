#version 460 core

struct Textures
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    sampler2D emission;
};
uniform Textures textures;

in vec2 TexCoord;



in vec3 viewfragPos;
in vec3 viewlightPos;
in vec3 normals;

in vec3 lightViewFragPos;



uniform sampler2D shadowsTexture;
float CalculateShadows()
{
    vec3 ndcCoords = lightViewFragPos / 2 + 0.5;

    float closestDepth = texture(shadowsTexture, ndcCoords.xy).r;

    float currentDepth = ndcCoords.z;

    return currentDepth > closestDepth ? 1.0 : 0.0;
}


vec3 CalculateLighting()
{
    // textures
    vec3 diffuseTex = vec3(1.0, 1.0, 1.0);
    diffuseTex = texture(textures.diffuse, TexCoord).rgb;
    
    vec3 specularTex = vec3(0.0, 0.0, 0.0);
    specularTex = texture(textures.specular, TexCoord).rgb;

    vec3 emissionTex = vec3(1.0, 1.0, 0.0);
    emissionTex = texture(textures.emission, TexCoord).rgb;

    // lights
    const float lightColorAmbient = 0.25;
    const vec3 lightColorDiffuse = vec3(0.8, 0.8, 0.8);
    const vec3 lightColorSpecular = vec3(1.0, 1.0, 1.0);


    const vec3 lightDirection = normalize(viewlightPos - viewfragPos);

    // diffuse
    const float dotProduct = dot(normals, lightDirection);
    const float diffuseLightPower = 5.0;
    float diffuseLight = max(dotProduct, 0.0);
    vec3 diffuseVec = (diffuseLight * diffuseLightPower * diffuseTex) * lightColorDiffuse;

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
    const float distance = length(viewlightPos - viewfragPos);
    const float attenuation = 1.0 / (distance);

    diffuseVec *= attenuation;
    specularVec *= attenuation;


    const float shadow = CalculateShadows();

    vec3 res = ambientVec + diffuseVec + specularVec + emissionTex * (1.0 - shadow);
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