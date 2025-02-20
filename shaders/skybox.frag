#version 460 core

struct Textures
{
    sampler2D emission;
};
uniform Textures textures;


in vec2 TexCoord;

out vec4 FragColor;
void main()
{
    FragColor = vec4(texture(textures.emission, TexCoord).rgb, 1.0);
}