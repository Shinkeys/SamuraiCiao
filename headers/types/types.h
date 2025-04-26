#pragma once
#include <glad/glad.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../window.h"

#include <vector>
#include <unordered_set>
#include <optional>
#include <queue>

class Shader;


// for RENDERING CLASS
enum class EntityType
{
    TYPE_NONE,
    TYPE_BOX_MESH,
    TYPE_COMPOUND_STATIC_MESH,
    TYPE_COMPOUND_DYNAMIC_MESH,
    TYPE_SKYBOX,
};  

namespace SamuraiDefines
{
    inline float g_fov       = 45.0f;
    inline float g_nearPlane = 0.002f;
    inline float g_farPlane  = 500.0f;
    inline double g_Pi       = 3.141592653589793;
    inline double g_TwoPi    = 6.28318530717958647692528676655900576;
};


struct ObjectDescriptor
{
    std::string name = "";
    EntityType type = EntityType::TYPE_NONE;
};

class Vertex
{
public:
    glm::vec3 position  = glm::vec3(0.0f);
    glm::vec3 normal    = glm::vec3(0.0f);
    glm::vec3 tangents  = glm::vec3(0.0f);
    glm::vec2 texCoords = glm::vec3(0.0f);

    Vertex& operator/=(float divider)
    {
        position.x /= divider;
        position.y /= divider;
        position.z /= divider;


        return *this;
    }
    Vertex& operator*=(float divider)
    {
        position.x *= divider;
        position.y *= divider;
        position.z *= divider;


        return *this;
    }
};

enum class LightType
{
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,
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

    int32_t type = 0x00;
};


struct EBOSetupFloat
{
    std::vector<float> vertices;
    std::vector<uint32_t> indices;
    uint32_t VAO{0};
    uint32_t VBO{0};
    uint32_t EBO{0};
};

struct EBOSetupUnskinned
{
    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;
    uint32_t VAO{0};
    uint32_t VBO{0};
    uint32_t EBO{0};
};

struct EBOSetupBuffers
{
    uint32_t VAO{0};
    uint32_t VBO{0};
    uint32_t EBO{0};
};

struct VBOSetupUnskinned
{
    std::vector<glm::vec3> vertices;
    uint32_t VAO{0};
    uint32_t VBO{0};

    int32_t type = 0x00;
};  


struct VBOSetupUnskinnedVec4
{
    std::vector<glm::vec4> vertices;
    uint32_t VAO{0};
    uint32_t VBO{0};

    int32_t type = 0x00;
};  


struct SSBOBind
{
    const uint32_t*  binding = nullptr;


    int32_t type = 0x00;
    int32_t size = 0;
    glm::vec3* data;
    uint32_t* ssboId = nullptr;
};

struct SSBOBindVec4
{
    const uint32_t*  binding = nullptr;

    int32_t type = 0x00;
    int32_t size = 0;
    glm::vec4* data;
    uint32_t* ssboId = nullptr;
};

// Purpose: class to handle object colors by values
enum class ColorsType
{
    DEFAULT,
    RED,
    BLUE,
    GREEN,
};
class ObjColor
{
private:
    glm::vec4 _color{1.0f, 1.0f, 1.0f, 1.0f};
public:
    glm::vec4 GetColor() const { return _color; }
    ObjColor& operator=(ColorsType color)
    {
        switch(color)
        {
        case ColorsType::DEFAULT:
            _color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        
        case ColorsType::RED:
            _color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
            break;
        
        case ColorsType::GREEN:
            _color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
            break;
        
        case ColorsType::BLUE:
            _color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
            break;
        
        default:
            std::cout << "Unable to recognize color of object\n";
            break;
        }

        return *this;
    }

};