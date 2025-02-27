#include "../../headers/scene/lanterns.h"
#include "../../headers/systems/renderManager.h"

void Lanterns::Prepare(AssetManager& manager)
{
    const std::string lanternObjectName = "Sun.glb";
        
    // manager.AddEntityToLoad(lanternObjectName);
    // glm::mat4 model = glm::mat4(1.0f);

    // _lightSourcePosition = glm::vec3(-20.0f, 10.0f, 0.0f);
    // model = glm::translate(model, _lightSourcePosition);
    // model = glm::scale(model, glm::vec3(15.0f));
    // manager.ApplyTransformation(lanternObjectName, model);
        
    // RenderManager::DispatchMeshToDraw(lanternObjectName, manager, EntityType::TYPE_MESH);

    manager.AddLightSourcePos(lanternObjectName, _lightSourcePosition);
}


glm::vec3 Lanterns::LightPositionViewSpace(const Matrices& matrices)
{
    glm::vec3 lightDirectionView = glm::vec3(-1.0f, -0.5f, -0.0f);
    lightDirectionView = matrices.view * glm::vec4(lightDirectionView, 1.0f);

    return lightDirectionView;
}





