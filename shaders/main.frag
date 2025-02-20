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


out vec4 FragColor;
void main()
{
    vec3 diffuse = vec3(1.0, 1.0, 1.0);
    // if(textureSize(textures.diffuse, 0).x > 0)
    diffuse = texture(textures.diffuse, TexCoord).rgb;
    
    vec3 specular = vec3(1.0, 1.0, 0.0);
    // if(textureIds.specular > -1)
    // {
    specular = texture(textures.specular, TexCoord).rgb;
    // }
    vec3 emission = vec3(1.0, 1.0, 0.0);
    // if(textureIds.emission > 0)
    // {
    emission = texture(textures.emission, TexCoord).rgb;
    // }

    vec3 result = diffuse + specular + emission;

    FragColor = vec4(result, 1.0);
}