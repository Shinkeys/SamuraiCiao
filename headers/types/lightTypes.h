#pragma once
#include "types.h"


// To Do: smart radius distribution
struct LightDescriptor
{
    enum class LightType
    {
        LIGHT_DIRECTIONAL,
        LIGHT_POINT,
    };
    glm::vec3 data; // should be or direction or position, depending on the type
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
    float   radius  = 0.0f; // used only for point light
    LightType type;
    bool affectsShadows = false;
};

// Purpose: there is a maximum of 4 lights influencing current scene shadows
// this struct is a handle for em
struct LightsAffectingShadowsDesc
{
    glm::vec3 lightsShadowsData = glm::vec3(-100.0f);
    int32_t lightForShadowsPresence = -1; // If > 0, then light data is filled
};