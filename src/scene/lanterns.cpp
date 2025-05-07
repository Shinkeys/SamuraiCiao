#include "../../headers/scene/lanterns.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/camera.h"

void Lanterns::Prepare(AssetManager& manager)
{
    ObjectDescriptor lanterObject;
    lanterObject.name = "Sun.glb";
    lanterObject.type = EntityType::TYPE_COMPOUND_STATIC_MESH;
    // manager.AddEntityToLoad(lanternObjectName);
    glm::mat4 model = glm::mat4(1.0f);

    // const glm::vec3 imaginarySunPosition = glm::vec3(0.0f, 3.0f, 155.0f);
    // // model = glm::translate(model, imaginarySunPosition);
    // // model = glm::scale(model, glm::vec3(15.0f));
    // // manager.ApplyTransformation(lanternObjectName, model);
        
    // RenderManager::DispatchMeshToDraw(lanterObject, manager);

    // attaching vector to the shader
    const std::string lightDirShaderName = "vsInput.viewlightDir";
    const std::string lightPosShaderName = "vsInput.viewlightPos";

    VectorDesc vectorDesc;
    vectorDesc.data = _directionalLightDir;
    vectorDesc.name = lightDirShaderName;
    vectorDesc.type = RenderPassType::RENDER_MAIN;

        
    RenderManager::AttachVectorToBind(vectorDesc);

    LightDescriptor lightDesc;
    lightDesc.name = lanterObject.name;
    lightDesc.type = LightDescriptor::LightType::LIGHT_DIRECTIONAL;
    lightDesc.influenceScene = true;
    lightDesc.data = _directionalLightDir;
    AddLightSourceDesc(lightDesc);


    lightDesc.name = "Lantern.gltf";
    lightDesc.type = LightDescriptor::LightType::LIGHT_POINT;
    lightDesc.influenceScene = true;
    lightDesc.data = glm::vec3(5.0f, 5.0f, 15.0f);
    lightDesc.color = glm::vec3(0.28f, 0.12f, 0.60f);
    AddLightSourceDesc(lightDesc);

}

void Lanterns::AddLightSourceDesc(const LightDescriptor& lightDesc)
{
    // To do: light remove from scene
    _lightsStorage.push_back(lightDesc);
}






