#include "../../headers/scene/temple.h"

Temple::Temple(AssetManager& manager) : _assetManager{manager}
{
    
}


void Temple::Prepare(Shader& shader)
{
    const std::string templeObjectName = "scene.gltf";
    
    _assetManager.AddEntityToLoad(templeObjectName, shader);
    Matrices matrices;
    matrices.model = glm::translate(matrices.model, glm::vec3(0.0f, 0.0f, -50.0f));
    matrices.model = glm::scale(matrices.model, glm::vec3(100.0f));
    _assetManager.ApplyMVPMatrices(templeObjectName, matrices);
}