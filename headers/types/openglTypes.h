#pragma once
#include "types.h"
#include "lightTypes.h"

struct DepthFramebuffer
{
    uint32_t buffer{ 0 };
    uint32_t texture{ 0 };

    glm::vec2 size{ 0, 0 };
};

//struct DepthFramebufferShadows
//{
//    uint32_t buffer {0};
//    std::array<uint32_t, LightDefines::g_max_lights_affecting_shadows> textures;
//
//    glm::vec2 size {0, 0};
//};