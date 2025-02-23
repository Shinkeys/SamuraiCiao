#include "../../headers/scene/lanterns.h"


void Lanterns::Prepare(AssetManager& manager, Shader& shader)
{
    const std::string lanternObjectName = "lantern.gltf";
    
    manager.AddEntityToLoad(lanternObjectName, shader);
    _lightSourcePosition = glm::vec3(-5.0f, 0.0f, -3.0f);
    glm::mat4 model;
    model = glm::translate(model, _lightSourcePosition);
    model = glm::scale(model, glm::vec3(5.0f));
    manager.ApplyTransformation(lanternObjectName, model);
    manager.AddLightSourcePos(lanternObjectName, _lightSourcePosition);
}


glm::vec3 Lanterns::LightPositionViewSpace(const Matrices& matrices)
{
    glm::vec3 lightSourcePosView;
    const glm::vec3 lampOffset = glm::vec3(4.0f, 10.0f, 0.0f);
    lightSourcePosView = matrices.view
    * glm::vec4(_lightSourcePosition + lampOffset, 1.0f);

    return lightSourcePosView;
}





