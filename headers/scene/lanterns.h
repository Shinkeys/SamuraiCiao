#pragma once
#include "../types/types.h"
#include "../systems/shaders.h"
#include "../assets/assetManager.h"



class Lanterns
{
private:
    std::vector<LightDescriptor> _lightsStorage;
    glm::vec3 _directionalLightDir = glm::vec3(0.0f, -1.0f, -2.0f);

    const uint32_t maxInfluencingLights = 512;
public:
    const auto& GetLightSources() const { return _lightsStorage;}
    void AddLightSourceDesc(const LightDescriptor& lightDesc);
    void Prepare(AssetManager& manager);
};