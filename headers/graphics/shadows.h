#pragma once
#include "../types/types.h"

#include "../../headers/assets/assetManager.h"
#include "../systems/shaders.h"
#include "lightSources.h"
#include "../systems/camera.h"
#include "../types/openglTypes.h"

class ShadowsHelper
{
private:
    // to rework
    const float _dirLightNearPlane = 0.1f;
    const float _dirLightFarPlane = 500.0f;

    const float _pointLightNearPlane = 0.07f;
    std::pair<uint32_t, uint32_t> _shadowTexExtent{0, 0};
    LightSources* _lightSources = nullptr;
    DepthFramebuffer _frameBuffer;
public:

    void PassLanterns(LightSources* lant);
    void MakeDynamicArea(float zNear, float zFar);
    void Prepare();
    void DebugShadows();
    void DrawDepthScene(AssetManager& manager);
};