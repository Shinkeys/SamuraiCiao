#include "../../headers/scene/lanterns.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/camera.h"

void Lanterns::Prepare(AssetManager& manager)
{
    const std::string lanternObjectName = "Sun.glb";
        
    manager.AddEntityToLoad(lanternObjectName);
    glm::mat4 model = glm::mat4(1.0f);

    const glm::vec3 imaginarySunPosition = glm::vec3(0.0f, 50.0f, 75.0f);
    model = glm::translate(model, imaginarySunPosition);
    model = glm::scale(model, glm::vec3(0.5f));
    manager.ApplyTransformation(lanternObjectName, model);
        
    RenderManager::DispatchMeshToDraw(lanternObjectName, manager, EntityType::TYPE_MESH);

    // attaching vector to the shader
    const std::string lightDirShaderName = "vsInput.viewlightDir";
    VectorDesc vectorDesc;
    vectorDesc.data = _directionalLightDir;
    vectorDesc.name = lightDirShaderName;
    vectorDesc.type = RenderPassType::RENDER_MAIN;
    if(auto shader = RenderManager::_shaderTypes.find(RenderPassType::RENDER_MAIN); shader != RenderManager::_shaderTypes.end())
        vectorDesc.shader = &shader->second; 
        
    RenderManager::AttachVectorToBind(vectorDesc);

    AddLightSourcePos(lanternObjectName, {LightType::LIGHT_DIRECTIONAL, _directionalLightDir});
}

void Lanterns::AddLightSourcePos(const std::string entityName, std::pair<LightType, glm::vec3> data)
{
    if(_lightSourcesData.find(entityName) == _lightSourcesData.end())
    {
        _lightSourcesData.insert({entityName, data});
    }
}






