#pragma once
#include "../types/types.h"

#include "../../headers/assets/assetManager.h"
#include "../systems/shaders.h"
#include "../scene/lanterns.h"
#include "../systems/camera.h"

class ShadowsHelper
{
private:
    // to rework
    const float _nearPlane = 0.01f;
    const float _farPlane = 10.0f;
    uint32_t _depthFBO;
    uint32_t _depthTex;
    std::pair<uint32_t, uint32_t> _shadowTexExtent;
    Lanterns* _lanterns = nullptr;
public:
    void PassLanterns(Lanterns* lant);
    void Prepare();
    void DebugShadows();
    void DrawDepthScene(AssetManager& manager, const Camera& camera);
};