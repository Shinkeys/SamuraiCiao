#version 460 core


layout(binding = 4) uniform sampler2D emission;


in vec2 TexCoord;

out vec4 FragColor;
void main()
{
    FragColor = vec4(texture(emission, TexCoord).rgb, 1.0);
}