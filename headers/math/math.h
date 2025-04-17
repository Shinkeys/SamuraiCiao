#pragma once
#include "../types/types.h"


namespace SamuraiMath
{
    bool IntersectAABB(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 boxMin, glm::vec3 boxMax);
};