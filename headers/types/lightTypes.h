#pragma once
#include "types.h"


enum class LightType : int32_t
{
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,

    LIGHT_TYPES_COUNT,
};
// To Do: smart radius distribution
struct LightDescriptor
{
    // VEC4 used to ADJUST it to the std430 layout on gpu which creates padding to 16 for every vec3
    glm::vec4 data = glm::vec4(-100.0f, -100.0f, -100.0f, -100.0f); // should be or direction or position, depending on the type
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    float   radius  = 15.0f; // used only for point light
    LightType type  = LightType::LIGHT_POINT;

    int32_t shadowsDataIndex = -1;

    int32_t padding = 0;
};

// Purpose: there is a maximum of 4 lights influencing current scene shadows
// this struct is a handle for em
struct LightsAffectingShadowsDesc
{
    glm::vec3 lightsShadowsData = glm::vec3(-100.0f);
    bool affectingShadows = false; // If > 0, then light data is filled
};


namespace LightDefines
{
    inline constexpr int32_t g_max_lights_per_tile = 128;
    inline constexpr int32_t g_max_lights_affecting_shadows = 4;
    inline constexpr int32_t g_max_lights_simultaneously = 1024;
};