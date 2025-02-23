#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

out vec2 TexCoord;
struct VSInput
{
    vec3 viewlightPos;
};

uniform VSInput vsInput;


out vec3 viewfragPos;
out vec3 viewlightPos;
out vec3 normals;

void main()
{
    gl_Position  = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    viewlightPos = vsInput.viewlightPos;
    viewfragPos = vec3(view * model * vec4(aPos, 1.0));
    normals = mat3(normalMatrix) * aNormal;
}
