#pragma once
#include "types.h"

struct DepthFramebuffer
{
    uint32_t buffer {0};
    uint32_t texture{0};

    glm::vec2 size {0, 0};
};