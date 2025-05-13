#include "../../headers/scene/temple.h"
#include "../../headers/systems/renderManager.h"
Temple::Temple(AssetManager& manager) : _assetManager{manager}
{
    
}


void Temple::Prepare(Shader& shader)
{
    ObjectDescriptor templeObject;
    templeObject.name = "temple";
    templeObject.fileName = "scene.gltf";
    templeObject.type = EntityType::TYPE_BOX_MESH;
    templeObject.storageFolder = "Temple";
    _assetManager.AddEntityToLoad(templeObject);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(100.0f));
    _assetManager.ApplyTransformation(templeObject.name, model);

    RenderManager::DispatchMeshToDraw(templeObject, _assetManager);
}