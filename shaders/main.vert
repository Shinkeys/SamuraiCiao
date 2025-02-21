#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
struct VSInput
{
    vec3 viewlightPos;
};

uniform VSInput vsInput;

struct VSOutput
{
    vec3 viewfragPos;
    vec3 viewlightPos;
    vec3 normals;
};

out VSOutput vsOutput;

void main()
{
    gl_Position  = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    vsOutput.viewlightPos = vsInput.viewlightPos;
    vsOutput.viewfragPos = vec3(view * vec4(aPos, 1.0));
    vsOutput.normals = aNormal;
}
