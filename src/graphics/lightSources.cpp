#include "../../headers/graphics/lightSources.h"

#include "imgui.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/camera.h"

void LightSources::Prepare(AssetManager& manager)
{
    ObjectDescriptor sunObject;
    sunObject.name = "sun.gltf";
    sunObject.type = EntityType::TYPE_COMPOUND_STATIC_MESH;

    VectorDesc vectorDesc;
    vectorDesc.data = _directionalLightDir;
    vectorDesc.name = lightDirShaderName;
    vectorDesc.type = RenderPassType::RENDER_MAIN;

        
    RenderManager::AttachVectorToBind(vectorDesc);

    LightDescriptor lightDesc;
    std::string lightName = "Sun";
    lightDesc.type = LightDescriptor::LightType::LIGHT_DIRECTIONAL;
    lightDesc.data = _directionalLightDir;
    lightDesc.affectsShadows = true;
    AddLightSourceDesc(lightDesc, lightName);


    lightName = "Lantern";
    lightDesc.type = LightDescriptor::LightType::LIGHT_POINT;
    lightDesc.data = glm::vec3(5.0f, 5.0f, 15.0f);
    lightDesc.color = glm::vec3(0.28f, 0.12f, 0.60f);
    lightDesc.radius = 5.0f;
    lightDesc.affectsShadows = false;

    AddLightSourceDesc(lightDesc, lightName);

}

// Purpose: method to work with imgui to create new lights
void LightSources::InterfaceLightsCreation()
{
    static bool createLightMode = false;
    ImGui::Checkbox("Create Lights", &createLightMode);
    if (createLightMode)
    {


        ImGui::Text("Light creation data:");
        ImGui::NewLine();
        ImGui::InputText("Name: ", _createLightDesc.nameBuffer, sizeof(_createLightDesc.nameBuffer));
        ImGui::InputFloat3("Position: ", _createLightDesc.position, "%.1f", 0);
        ImGui::InputFloat3("Color: ", _createLightDesc.color, "%.1f", 0);

        if (ImGui::Button("Create"))
        {
            if (_createLightDesc.nameBuffer[0] == '\0')
            {
                std::cout << "Can't create light, name is empty\n";
                return;
            }

            LightDescriptor lightDesc;

            auto lightName = std::string(_createLightDesc.nameBuffer);
            lightDesc.data = glm::vec3(_createLightDesc.position[0], _createLightDesc.position[1], _createLightDesc.position[2]);
            lightDesc.color = glm::vec3(_createLightDesc.color[0], _createLightDesc.color[1], _createLightDesc.color[2]);

            AddLightSourceDesc(lightDesc, lightName);
        }

    }
}

void LightSources::AddLightSourceWithShadowInfluence(const std::string& lightName, glm::vec3 lightData)
{
    auto duplicateIt  = std::find_if(_lightsForShadowsStorage.begin(), _lightsForShadowsStorage.end(),  [&lightName](const auto& pair)
    {
        return pair.first == lightName;
    });
    if (duplicateIt != _lightsForShadowsStorage.end())
    {
        duplicateIt->second.lightsShadowsData = lightData;
        duplicateIt->second.lightForShadowsPresence = 1;
        return;
    }

    auto freeSpaceIt = std::find_if(_lightsForShadowsStorage.begin(), _lightsForShadowsStorage.end(), [](const auto& pair)
    {
        return pair.second.lightForShadowsPresence <= 0;
    });
    if (freeSpaceIt != _lightsForShadowsStorage.end())
    {
        freeSpaceIt->second.lightsShadowsData = lightData;
        freeSpaceIt->second.lightForShadowsPresence = 1;
        return;
    }

    // No duplicates in array and no free space found, change the farthest light on the new one
    float newLightDistance =  glm::length(lightData);
    int32_t newIndex  = -1;
    for (int32_t i = 0; i < _lightsForShadowsStorage.size(); ++i)
    {
        const float currentDistance = glm::length(_lightsForShadowsStorage[i].second.lightsShadowsData);
        if (currentDistance > newLightDistance)
        {
            newLightDistance = currentDistance;
            newIndex = i;
        }
    }

    if (newIndex == -1)
        return;

    _lightsForShadowsStorage[newIndex].first = lightName;
    _lightsForShadowsStorage[newIndex].second.lightsShadowsData = lightData;
    _lightsForShadowsStorage[newIndex].second.lightForShadowsPresence = 1;
}

void LightSources::AddLightSourceDesc(const LightDescriptor& lightDesc, const std::string& lightName)
{
    auto lightIt = _lightsStorage.find(lightName);
    if (lightIt != _lightsStorage.end())
    {
        const auto& light = lightIt->second;
        if (light.data.x == lightDesc.data.x && light.data.y == lightDesc.data.y && light.data.z == lightDesc.data.z)
        {
            if (light.color.x == lightDesc.color.x && light.color.y == lightDesc.color.y && light.color.z == lightDesc.color.z)
            {
                std::cout << "Light with exact same data already exist\n";
                return;
            }
        }
    }
    std::cout << "Created light source\n";
    // To do: light remove from scene
    _lightsStorage.insert({lightName, lightDesc});

    AddLightSourceWithShadowInfluence(lightName, lightDesc.data);
}






