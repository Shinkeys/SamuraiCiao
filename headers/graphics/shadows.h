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
    const float _nearPlane = 0.1f;
    const float _farPlane = 500.0f;
    uint32_t _depthFBO{0};
    uint32_t _depthTex{0};
    std::pair<uint32_t, uint32_t> _shadowTexExtent{0, 0};
    Lanterns* _lanterns = nullptr;
public:
    void PassLanterns(Lanterns* lant);
    void MakeDynamicArea(float zNear, float zFar);
    void Prepare();
    void DebugShadows();
    void DrawDepthScene(AssetManager& manager, const Camera& camera);
};