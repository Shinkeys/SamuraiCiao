#pragma once
#include "../types/types.h"
#include "../systems/shaders.h"
#include "../assets/assetManager.h"

class Skybox
{
private:
    AssetManager& _assetManager;
    uint32_t StbiLoadCubeTexture(std::vector<std::string> entityNames);
public:
    Skybox() = default;
    Skybox(AssetManager& manager);
    void Prepare();
};