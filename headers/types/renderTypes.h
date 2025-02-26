#include "../types/types.h"

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
};
// basically struct to make it more convenient to operate later
struct TextureDesc
{  
    RenderPassType type;
    std::string name;
    uint32_t handle;

    Shader* shader = nullptr;
};