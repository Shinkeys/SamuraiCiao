#pragma once
#include "../types/lightTypes.h"
#include "../systems/shaders.h"
#include "../assets/assetManager.h"


// Would use char to make better compatibility with imgui
struct CreateLightDesc
{
    char nameBuffer[33];
    float position[3];
    float color[3];
};

class LightSources
{
private:
    std::unordered_map<std::string, LightDescriptor> _lightsStorage;
    glm::vec3 _directionalLightDir = glm::vec3(0.0f, 1.0f, 2.0f);

    std::array<std::pair<std::string, LightsAffectingShadowsDesc>, 4> _lightsForShadowsStorage;
    CreateLightDesc _createLightDesc { };
    const uint32_t maxInfluencingLights = 512;

    void AddLightSourceDesc(const LightDescriptor& lightDesc, const std::string& lightName);
    // If this light source affects shadows in the scene
    void AddLightSourceWithShadowInfluence(const std::string& lightName, glm::vec3 lightData);
public:
    void InterfaceLightsCreation();
    const auto& GetLightSources() const { return _lightsStorage;}
    const auto& GetLightsInfluencingShadows() const { return _lightsForShadowsStorage; }
    void Prepare(AssetManager& manager);
};