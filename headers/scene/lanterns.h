#pragma once
#include "../types/types.h"
#include "../systems/shaders.h"
#include "../assets/assetManager.h"



class Lanterns
{
private:
    std::unordered_map<std::string, std::pair<LightType, glm::vec3>> _lightSourcesData;
    uint32_t _lightSourcesCount = 0;
    glm::vec3 _directionalLightDir = glm::vec3(0.0f, -1.0f, -2.0f);
public:
    const auto& GetLightSourcesData() const { return _lightSourcesData;}
    void AddLightSourcePos(const std::string entityName, std::pair<LightType, glm::vec3> data);
    void Prepare(AssetManager& manager);
};