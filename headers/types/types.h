#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../window.h"

#include <vector>

class Shader;

namespace SamuraiDefines
{
    inline float g_fov = 45.0f;
    inline float g_nearPlane = 0.002f;
    inline float g_farPlane = 500.0f;
};


struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};


struct Matrices // MVP matrices for current camera
{
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f));
    glm::mat4 projection = glm::perspective(SamuraiDefines::g_fov, 
        static_cast<float>(2560) // to do
        / static_cast<float>(1440),
        SamuraiDefines::g_nearPlane, SamuraiDefines::g_farPlane);
};

struct ModelTexDesc
{
    int32_t diffuseId{0};
    int32_t specularId{0};
    int32_t normalId{0};
    int32_t emissionId{0};
};
struct EBOSetup
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t VAO{0};
    uint32_t VBO{0};
    uint32_t EBO{0};
};
