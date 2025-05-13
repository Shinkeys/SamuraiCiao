#pragma once

#include "../types/types.h"
#include <variant>



enum class RenderPassType
{
    RENDER_MAIN = 1 << 0,
    RENDER_SKYBOX = 1 << 1,
    RENDER_DEPTHPASS = 1 << 2,
    RENDER_PARTICLES = 1 << 3,
    RENDER_PARTICLES_COMP = 1 << 4,
    RENDER_SCENE_EDITOR = 1 << 5,
    RENDER_COLLISION_DEBUG = 1  << 6,

    RENDER_ALL = 1 << 7,
};



// basically struct to make it more convenient to operate later
struct TextureDesc
{  
    RenderPassType type;
    std::string name;
    uint32_t handle;
    uint32_t bindSlot;

    bool operator==(const TextureDesc& other) const { return name == other.name && type == other.type;}
};
// matrix desc to bind needed matrices in render call
struct MatrixDesc
{
    RenderPassType type;
    std::string name;
    glm::mat4 data;

    // operator overload for set
    bool operator==(const MatrixDesc& other) const { return name == other.name && type == other.type;}
};



struct VectorDesc
{  
    RenderPassType type;
    std::string name;
    std::variant<glm::vec2, glm::vec3, glm::vec4> data;

    bool operator==(const VectorDesc& other) const { return name == other.name && type == other.type;}
};

// to do: template hasher
struct TextureHashFunc
{
    size_t operator()(const TextureDesc& type) const
    {
        size_t hashFirst = std::hash<std::string>()(type.name);
        size_t hashSecond = std::hash<RenderPassType>()(type.type);
        return hashFirst ^ (hashSecond << 1);
    }
};
struct MatrixHashFunc
{
    size_t operator()(const MatrixDesc& type) const
    {
        size_t hashFirst = std::hash<std::string>()(type.name);
        size_t hashSecond = std::hash<RenderPassType>()(type.type);
        return hashFirst ^ (hashSecond << 1);
    }
};
struct VectorHashFunc
{
    size_t operator()(const VectorDesc& type) const
    {
        size_t hashFirst = std::hash<std::string>()(type.name);
        size_t hashSecond = std::hash<RenderPassType>()(type.type);
        return hashFirst ^ (hashSecond << 1);
    }
};

