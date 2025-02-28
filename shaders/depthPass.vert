#version 460 core
layout(location = 0) in vec3 aPos;


uniform mat4 lightMatrix;
uniform mat4 model;

void main()
{
    vec3 pos = aPos;
    gl_Position = lightMatrix * model * vec4(pos, 1.0);
}