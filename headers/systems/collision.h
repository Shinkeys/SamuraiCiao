#include "../types/types.h"
#include "../assets/assetManager.h"

struct AABB
{
    std::string entityName = "";
    glm::vec3 min{glm::vec3(std::numeric_limits<float>::max())};
    glm::vec3 max{glm::vec3(std::numeric_limits<float>::lowest())};
};

struct CollisionData
{
    std::vector<glm::vec3> vertices;
    uint32_t offset = 0;
};

class Collision
{
private:
    Shader _debugShader;

    const uint32_t _vectorDim = 3;
    AssetManager* _manager = nullptr;
    VBOSetupUnskinned _setup;
    std::unordered_map<std::string, CollisionData> _collisionStorage;
    void CheckForCollision(glm::vec3 meshPos);
    void AddAABBForModel(const AABB& aabb);
public:
    void VisualizeAABB(const glm::mat4& view, const glm::mat4& projection);
    void UpdateAABB();
    void PassAssetManager(AssetManager& manager);
    void Prepare();

};