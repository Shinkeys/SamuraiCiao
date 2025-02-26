#pragma once
#include "../types/types.h"

#include "../../headers/assets/assetManager.h"
#include "../systems/shaders.h"


class ShadowsHelper
{
private:
    uint32_t _depthFBO;
    uint32_t _depthTex;
    std::pair<uint32_t, uint32_t> _shadowTexExtent;
public:
    void Prepare();
    void DebugShadows();
    void DrawDepthScene(AssetManager& manager);
};