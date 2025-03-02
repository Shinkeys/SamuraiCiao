#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;

uniform mat4 lightMatrix;

out vec2 TexCoord;
struct VSInput
{
    vec3 viewlightDir;
};

uniform VSInput vsInput;

out vec3 viewfragPos;
out vec3 viewlightDir;
out vec3 normals;
out vec3 lightViewFragPos;

out vec3 lightViewLightDir;
void main()
{
    gl_Position  = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    viewlightDir = vec3(mat3(view) * vsInput.viewlightDir);
    // to check
    lightViewLightDir = vec3(mat3(lightMatrix) * vsInput.viewlightDir);
    viewfragPos = vec3(view * model * vec4(aPos, 1.0));
    normals = mat3(normalMatrix) * aNormal;
    lightViewFragPos = vec3(lightMatrix * model * vec4(aPos, 1.0));
}
