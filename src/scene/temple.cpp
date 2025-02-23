#include "../../headers/scene/temple.h"

Temple::Temple(AssetManager& manager) : _assetManager{manager}
{
    
}


void Temple::Prepare(Shader& shader)
{
    const std::string templeObjectName = "scene.gltf";
    
    _assetManager.AddEntityToLoad(templeObjectName, shader);
    glm::mat4
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -25.0f));
    model = glm::scale(model, glm::vec3(100.0f));
    _assetManager.ApplyTransformation(templeObjectName, model);
}