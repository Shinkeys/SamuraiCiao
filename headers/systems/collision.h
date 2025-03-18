#pragma once

#include "../types/types.h"
#include "../assets/assetManager.h"
#include "../systems/camera.h"

struct AABB
{
    std::string entityName = "";
    glm::vec3 min{glm::vec3(std::numeric_limits<float>::max())};
    glm::vec3 max{glm::vec3(std::numeric_limits<float>::lowest())};
};

// TO DO: as part of CollisionData struct, now test it like that
struct Sphere
{
    std::string entityName = "";
    glm::vec3 center{glm::vec3(0.0f)};
    float radius = 0.0f;
};

enum class CollisionType
{
    COLLISION_AABB,
    COLLISION_SPHERE,
};


// making these structures separated to simplify code which is great overall in this situation
// as there's no need to create a lot of types of collision, about 2-3 types
struct AABBData
{
    std::vector<glm::vec3> vertices;
    uint32_t offset = 0;
    AABB aabbPoints;
};

struct SphereData
{
    uint32_t offset = 0;
    Sphere points;
};

class Collision
{
private:
    Shader _debugShader;

    const float _cameraSphereRadius = 1.0f;
    const uint32_t _vectorDim = 3;
    AssetManager* _manager = nullptr;
    VBOSetupUnskinned _setup;
    bool _visualizeAABB = false;
    std::unordered_map<std::string, AABBData> _collisionAABBStorage;
    std::unordered_map<std::string, SphereData> _collisionSphereStorage;
    std::unordered_map<std::string, CollisionType> _collisionTypesStorage;
    void CheckForCollision(glm::vec3 meshPos);
    bool CalculateCameraCollision(Camera& camera, const glm::vec3& objectCenter, const glm::vec3& objectExtent);
    void AddAABBForModel(const AABB& aabb);

    void CalculateModelAABB(const std::vector<Vertex>& vertices, const std::string& entityName);
    void CalculateModelSphere(const std::vector<Vertex>& vertices, const std::string& entityName, float radius);
    
public:
    void CheckCameraForCollision(Camera& camera);
    void VisualizeAABB(const glm::mat4& view, const glm::mat4& projection);
    void UpdateAABB();
    void PassAssetManager(AssetManager& manager);
    void Prepare();
    void AddCollisionTypeToObject(std::string entityName, CollisionType type);
    void EnableCollisionDisplay();

};