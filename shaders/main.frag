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
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);


    const vec3 lightDirection = normalize(viewlightPos - viewfragPos);

    // diffuse
    const float dotProduct = dot(normals, -lightDirection);
    float diffuseLight = max(dotProduct, 0.0);
    const vec3 diffuseVec = (diffuseLight * diffuseTex) * lightColor;

    // specular
    const vec3 viewDirection = normalize(-viewfragPos);
    const vec3 reflectDirection = reflect(-lightDirection, normals);
    const float specularLight = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    const vec3 specularVec = (specularLight * specularTex) * lightColor;

    // ambient
    const float ambient = 0.1;
    const vec3 ambientVec = ambient * diffuseTex;

    vec3 res = ambientVec + diffuseVec + specularVec + emissionTex;
    return res;
}



out vec4 FragColor;
void main()
{
    vec3 result = CalculateLighting();

    FragColor = vec4(result, 1.0);
}