#include "../types/types.h"
#include "../assets/assetManager.h"

struct AABB
{
    glm::vec3 min{glm::vec3(0.0f)};
    glm::vec3 max{glm::vec3(0.0f)};
};

class Collision
{
private:
    Shader _debugShader;

    AssetManager* _manager = nullptr;
    AABB _collisionBox; 
    EBOSetupUnskinned _setup;
    void CheckForCollision(glm::vec3 meshPos);
    void AddAABBForModel(const AABB& aabb);
public:
    void VisualizeAABB(const glm::mat4& view, const glm::mat4& projection);
    void UpdateAABB();
    void PassAssetManager(AssetManager& manager);
    void Prepare();

};