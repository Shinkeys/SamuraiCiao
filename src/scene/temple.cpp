#include "../../headers/scene/temple.h"
#include "../../headers/systems/renderManager.h"
Temple::Temple(AssetManager& manager) : _assetManager{manager}
{
    
}


void Temple::Prepare(Shader& shader)
{
    const std::string templeObjectName = "scene.gltf";
    
    _assetManager.AddEntityToLoad(templeObjectName);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(100.0f));
    _assetManager.ApplyTransformation(templeObjectName, model);

    RenderManager::DispatchMeshToDraw(templeObjectName, _assetManager, EntityType::TYPE_MESH);
}