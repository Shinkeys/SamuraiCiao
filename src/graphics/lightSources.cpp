#include "../../headers/graphics/lightSources.h"

#include "imgui.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/camera.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/types/random.h"
#include "../../headers/utilities/geometryUtil.h"

void LightSources::Prepare()
{
    LightDescriptor lightDesc;
    std::string lightName = "Red";
    


    /*lightDesc.type = LightType::LIGHT_POINT;
    lightDesc.data = glm::vec4(0.0f, 5.0f, 17.0f, 1.0f);
    lightDesc.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    AddLightSourceDesc(lightDesc, lightName, true);*/


    //lightName = "Green";
    //lightDesc.type = LightType::LIGHT_POINT;
    //lightDesc.data = glm::vec4(0.0f, 5.0f, -25.0f, 1.0f);
    //lightDesc.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    //AddLightSourceDesc(lightDesc, lightName, false);

    //lightName = "Blue";
    //lightDesc.type = LightType::LIGHT_POINT;
    //lightDesc.data = glm::vec4(0.0f, 5.0f, 10.0f, 1.0f);
    //lightDesc.color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    //AddLightSourceDesc(lightDesc, lightName, false);


    float lastPos = -150.0f;
    float lastX = -150.0f;
     for (int j = 0; j < 4; ++j)
     {
         for (int i = 0; i < 256; ++i)
         {
             lightName = lightName + std::to_string(j) + "_" + std::to_string(i);
             lightDesc.data = glm::vec4(Random::RandomFloat(-250.0, 250.0f), 5.0f, Random::RandomFloat(-250.0, 250.0f), 1.0f);
             
             lightDesc.type = LightType::LIGHT_POINT;
             lightDesc.color = glm::vec4(Random::RandomFloat(0.0f, 1.0f), Random::RandomFloat(0.0f, 1.0f), 
                 Random::RandomFloat(0.0f, 1.0f), 1.0f);
             AddLightSourceDesc(lightDesc, lightName, false);
    
             lastX += 15.0f;
             lastPos += 15.0f;
    
             
         }
    
     }


    CreateLightBuffers();
}


void LightSources::CreateLightBuffers()
{
    for (const auto& light : _lightsStorage)
    {
        _lightsBufferHandle.lights.push_back(light.second);
    }

    // Init SSBO
    _lightsBufferHandle.lightsBindID = 1;
    SSBOBind<LightDescriptor> bindData;
    bindData.binding = &_lightsBufferHandle.lightsBindID;
    bindData.ssboId = &_lightsBufferHandle.lightsHandle;
    bindData.data = _lightsBufferHandle.lights.data();
    bindData.size = static_cast<int32_t>(sizeof(LightDescriptor) * _lightsBufferHandle.lights.size());
    bindData.type = GL_DYNAMIC_STORAGE_BIT;
    if(OpenglBackend::CreateSSBOImmutable(bindData) == ErrorCodes_Backend::ERROR_SSBO_CREATION)
    {
        std::cout << "Unable to initialize light cull, ssbo is not created\n";
    }

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
        ImGui::Checkbox("Affect shadows: ", &_createLightDesc.affectsShadows);


        static LightType currentItemSelectedType = _createLightDesc.type;
        if (ImGui::BeginCombo("Select light type: ", ""))
        {
            constexpr int32_t lightTypesCount = static_cast<int32_t>(LightType::LIGHT_TYPES_COUNT);

            for (int32_t i = 0; i < lightTypesCount; ++i)
            {
                auto type = static_cast<LightType>(i);
                bool isSelected = (currentItemSelectedType == type);

                const char* name = nullptr;

                switch (type)
                {
                case LightType::LIGHT_DIRECTIONAL:
                    name = "Directional";
                    break;
                case LightType::LIGHT_POINT:
                    name = "Point";
                    break;
                default:
                    name = "Unknown";
                    return;
                }

                if (ImGui::Selectable(name, isSelected))
                    currentItemSelectedType = type;
                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }


        if (ImGui::Button("Create"))
        {
            if (_createLightDesc.nameBuffer[0] == '\0')
            {
                std::cout << "Can't create light, name is empty\n";
                return;
            }

            LightDescriptor lightDesc;

            auto lightName = std::string(_createLightDesc.nameBuffer);
            lightDesc.data = glm::vec4(_createLightDesc.position[0], _createLightDesc.position[1], _createLightDesc.position[2], 1.0f);
            lightDesc.color = glm::vec4(_createLightDesc.color[0], _createLightDesc.color[1], _createLightDesc.color[2], 1.0f);
            lightDesc.type = currentItemSelectedType;

            AddLightSourceDesc(lightDesc, lightName, _createLightDesc.affectsShadows);
        }

    }
}

void LightSources::AddLightSourceWithShadowInfluence(const std::string& lightName, LightDescriptor& lightDesc)
{
    auto duplicateIt  = std::find_if(_lightsForShadowsStorage.begin(), _lightsForShadowsStorage.end(),  [&lightName](const auto& pair)
    {
        return pair.first == lightName;
    });
    if (duplicateIt != _lightsForShadowsStorage.end())
    {
        duplicateIt->second.lightsShadowsData = lightDesc.data;
        duplicateIt->second.affectingShadows = true;
        duplicateIt->second.type = lightDesc.type;
        duplicateIt->second.radius = lightDesc.radius;
        return;
    }


    for (int32_t i = 0; i < _lightsForShadowsStorage.size(); ++i)
    {
        if (_lightsForShadowsStorage[i].second.affectingShadows == false)
        {
            _lightsForShadowsStorage[i].first = lightName;
            _lightsForShadowsStorage[i].second.lightsShadowsData = lightDesc.data;
            _lightsForShadowsStorage[i].second.affectingShadows = true;
            _lightsForShadowsStorage[i].second.type = lightDesc.type;
            _lightsForShadowsStorage[i].second.radius = lightDesc.radius;

            // Storing index into shadows storage buffer
            lightDesc.shadowsDataIndex = i;
            return;
        }
    }

    // No duplicates in the array and no free space found, change the farthest light on the new one
    float newLightDistance =  glm::length(lightDesc.data);
    int32_t newIndex  = -1;
    for (int32_t i = 0; i < _lightsForShadowsStorage.size(); ++i)
    {
        const float currentDistance = glm::length(_lightsForShadowsStorage[i].second.lightsShadowsData);
        if (currentDistance >= newLightDistance)
        {
            newLightDistance = currentDistance;
            newIndex = i;
        }
    }

    if (newIndex == -1)
        return;

    _lightsForShadowsStorage[newIndex].first = lightName;
    _lightsForShadowsStorage[newIndex].second.lightsShadowsData = lightDesc.data;
    _lightsForShadowsStorage[newIndex].second.type = lightDesc.type;
    _lightsForShadowsStorage[newIndex].second.radius = lightDesc.radius;

    // // removing state from the previous element
    // auto it = _lightsStorage.find(lightName);
    // if (it != _lightsStorage.end())
    // {
    //     it->second.shadowsDataIndex = -1;
    //     UpdateLightBuffer()
    // }
    lightDesc.shadowsDataIndex = newIndex;
}

void LightSources::UpdateLightBuffer(LightUpdateCommand command, const std::string& lightName)
{
    if (_lightsBufferHandle.lights.size() > LightDefines::g_max_lights_simultaneously)
    {
        return;
    }

    switch (command)
    {
    case LightUpdateCommand::ADD_LIGHT:
    {
        const auto it = _lightsStorage.find(lightName);
        if (it != _lightsStorage.end())
        {
            // if buffer is created
            if (_lightsBufferHandle.lightsHandle > 0)
            {
                _lightsBufferHandle.lights.push_back(it->second);
                const uint32_t offset = static_cast<uint32_t>(_lightsBufferHandle.lights.size() - 1) * sizeof(LightDescriptor);
                glNamedBufferSubData(_lightsBufferHandle.lightsHandle, offset, sizeof(LightDescriptor), &_lightsBufferHandle.lights.back());
            }
        }
    }
        break;

    case LightUpdateCommand::REMOVE_LIGHT:
        // TO DO
        break;
    case LightUpdateCommand::UPDATE_LIGHT:
        // TO DO
        break;

    default:
        std::cout << "No such a command for light buffer update\n";
    }



}

void LightSources::AddLightSourceDesc(LightDescriptor& lightDesc, const std::string& lightName, bool affectShadows)
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

    if (affectShadows)
        AddLightSourceWithShadowInfluence(lightName, lightDesc);
    else
        lightDesc.shadowsDataIndex = -1;

    std::cout << "Created light source\n";
    // To do: light remove from scene
    _lightsStorage.insert({lightName, lightDesc});

    UpdateLightBuffer(LightUpdateCommand::ADD_LIGHT, lightName);
}