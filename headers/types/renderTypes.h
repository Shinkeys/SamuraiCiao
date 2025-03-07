#include "../types/types.h"
#include <variant>
// for RENDERING CLASS
enum class EntityType
{
    TYPE_MESH,
    TYPE_SKYBOX
};  


enum class RenderPassType
{
    RENDER_MAIN,
    RENDER_SKYBOX,
    RENDER_DEPTHPASS,
    RENDER_PARTICLES,
    RENDER_PARTICLES_COMP,
    RENDER_SCENE_EDITOR
};
// basically struct to make it more convenient to operate later
struct TextureDesc
{  
    RenderPassType type;
    std::string name;
    uint32_t handle;

    Shader* shader = nullptr;
    bool operator==(const TextureDesc& other) const { return name == other.name;}
};
// matrix desc to bind needed matrices in render call
struct MatrixDesc
{
    RenderPassType type;
    std::string name;
    glm::mat4 data;

    Shader* shader = nullptr;

    // operator overload for set
    bool operator==(const MatrixDesc& other) const { return name == other.name;}
};



struct VectorDesc
{  
    RenderPassType type;
    std::string name;
    std::variant<glm::vec2, glm::vec3, glm::vec4> data;

    Shader* shader = nullptr;
    bool operator==(const VectorDesc& other) const { return name == other.name;}
};

// to do: template hasher
struct TextureHashFunc
{
    size_t operator()(const TextureDesc& type) const { return std::hash<std::string>()(type.name);}
};
struct MatrixHashFunc
{
    size_t operator()(const MatrixDesc& type) const { return std::hash<std::string>()(type.name);}
};


struct VectorHashFunc
{
    size_t operator()(const VectorDesc& type) const { return std::hash<std::string>()(type.name);}
};