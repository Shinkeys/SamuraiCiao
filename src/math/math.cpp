    #include "../../headers/math/math.h"

bool SamuraiMath::IntersectAABB(glm::vec3 rayOrigin, glm::vec3 rayDir, 
                    glm::vec3 boxMin, glm::vec3 boxMax)
{
    float t1 = (boxMin.x - rayOrigin.x) / rayDir.x;
    float t2 = (boxMax.x - rayOrigin.x) / rayDir.x;
    float t3 = (boxMin.y - rayOrigin.y) / rayDir.y;
    float t4 = (boxMax.y - rayOrigin.y) / rayDir.y;
    float t5 = (boxMin.z - rayOrigin.z) / rayDir.z;
    float t6 = (boxMax.z - rayOrigin.z) / rayDir.z;

    float tMin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tMax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));


    
    if(tMax < 0.0f)
    {
        return false;
    }

    if(tMin > tMax)
    {
        return false;
    }


    return true;
}

bool SamuraiMath::RayIntersectCylinder(glm::vec3 rayOrigin, glm::vec3 rayDir, float cylinderRadius, const std::vector<glm::vec3>& vertices)
{
    // float t0;
    // float t1;

    // const glm::vec3 L = rayOrigin - ;
    // float a = glm::dot(rayDir, rayDir);
    // float b = 2 * glm::dot(rayDir, L);
    // float c = glm::dot(L, L) - cylinderRadius * cylinderRadius;

    // if(!SolveQuadratic(a,b,c,t0,t1)) return false;

    // if(t0 > t1) std::swap(t0, t1);

    // if(t0 < 0)
    // {
    //     if(t1 < 0) return false;
    // }
    
    return false;
}

bool SamuraiMath::SolveQuadratic(float a, float b, float c, float& x0, float& x1)
{
    float discriminant = b * b - 4 * a * c;

    if(discriminant < 0) return false;

    else if(discriminant == 0)
    {
        x0 = -0.5f * b / a;
        x1 = -0.5f * b / a;
    }
    else
    {
        float q = (b > 0) ? -0.5f * (b + std::sqrt(discriminant)) :
                            -0.5f * (b - std::sqrt(discriminant));
        
        x0 = q / a;
        x1 = c / q;
    }
    if(x0 > x1) std::swap(x0, x1);

    return true;
}
