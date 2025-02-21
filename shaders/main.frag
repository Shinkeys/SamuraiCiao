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


in struct VSOutput
{
    vec3 viewfragPos;
    vec3 viewlightPos;
    vec3 normals;
} fsInput;


vec3 CalculateLighting(VSOutput data)
{
    // textures
    vec3 diffuseTex = vec3(1.0, 1.0, 1.0);
    diffuseTex = texture(textures.diffuse, TexCoord).rgb;
    
    vec3 specularTex = vec3(1.0, 1.0, 0.0);
    specularTex = texture(textures.specular, TexCoord).rgb;

    vec3 emissionTex = vec3(1.0, 1.0, 0.0);
    emissionTex = texture(textures.emission, TexCoord).rgb;

    // lights
    const vec3 lightColor = vec3(1.0, 1.0, 1.0);

    const float ambient = 0.35;
    const vec3 ambientVec = ambient * lightColor;

    const vec3 lightDirection = 
        normalize(data.viewfragPos - data.viewlightPos);

    // const float dotProduct = dot(lightDirection, data.normals);
    // float diffuseLight = max(dotProduct, 0.0);



    vec3 res = ambientVec * (diffuseTex + specularTex + emissionTex);

    return res;
}



out vec4 FragColor;
void main()
{
    vec3 result = CalculateLighting(fsInput);

    FragColor = vec4(result, 1.0);
}