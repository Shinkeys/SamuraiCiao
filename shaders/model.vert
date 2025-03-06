#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;

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
out vec3 lightViewFragPos;
out vec3 lightViewLightDir;


out vec3 normals;
void main()
{
    gl_Position  = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    // to check
    lightViewLightDir = vec3(mat3(lightMatrix) * vsInput.viewlightDir);
    lightViewFragPos = vec3(lightMatrix * model * vec4(aPos, 1.0));

    // normal mapping
    // calculating tangents, normals via normal matrix because if model would have heterogeneous
    // scale, would break normals, tangents etc
    normals = normalize(mat3(normalMatrix) * aNormal);
    vec3 tangents = normalize(mat3(normalMatrix) * aTangent);
    vec3 bitangents = normalize(cross(normals, tangents));
    // Gram-Schmidt process to make vectors orthogonal back
    tangents = normalize(tangents - dot(tangents, normals) * normals);

    mat3 TBN = transpose(mat3(tangents, bitangents, normals));
    
    viewlightDir = vec3(TBN * mat3(view * model) * vec3(vsInput.viewlightDir));
    viewfragPos = vec3(TBN * mat3(view * model) * vec3(aPos));
}
