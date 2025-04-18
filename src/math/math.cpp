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