    #include "../../headers/math/math.h"

    bool SamuraiMath::IntersectAABB(glm::vec3 rayOrigin, glm::vec3 rayDir, 
                    glm::vec3 boxMin, glm::vec3 boxMax)
    {
        float tMin = std::numeric_limits<float>::lowest();
        float tMax = std::numeric_limits<float>::max();

        if(rayDir.x != 0.0f)
        {
            float tx1 = (boxMin.x - rayOrigin.x) / rayDir.x;
            float tx2 = (boxMax.x - rayOrigin.x) / rayDir.x;

            tMin = std::max(tMin, std::min(tx1, tx2));
            tMax = std::min(tMax, std::max(tx1, tx2));
        }

        if(rayDir.y != 0.0f)
        {
            float ty1 = (boxMin.y - rayOrigin.y) / rayDir.y;
            float ty2 = (boxMax.y - rayOrigin.y) / rayDir.y;

            tMin = std::max(tMin, std::min(ty1, ty2));
            tMax = std::min(tMax, std::max(ty1, ty2));
        }

        if(rayDir.z != 0.0f)
        {
            float tz1 = (boxMin.z - rayOrigin.z) / rayDir.z;
            float tz2 = (boxMax.z - rayOrigin.z) / rayDir.z;

            tMin = std::max(tMin, std::min(tz1, tz2));
            tMax = std::min(tMax, std::max(tz1, tz2));
        }

        return tMax >= tMin;
    }