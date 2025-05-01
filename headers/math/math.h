#pragma once
#include "../types/types.h"


namespace SamuraiMath
{
    bool IntersectAABB(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 boxMin, glm::vec3 boxMax);
    bool RayIntersectCylinder(glm::vec3 rayOrigin, glm::vec3 rayDir, float cylinderRadius, const std::vector<glm::vec3>& vertices);
    bool SolveQuadratic(float a, float b, float c, float& x0, float& x1);
};