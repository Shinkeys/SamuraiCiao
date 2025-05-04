#pragma once
#include "../types/renderTypes.h"
#include "../types/types.h"
#include "../types/openglTypes.h"
#include "shaders.h"


struct ForwardCompInitializers
{
    int32_t tileSize = 16;
    int32_t numGroupsX = 128;
    int32_t numGroupsY = 128;
};

class AssetManager;
class ForwardPlusRender
{
private:
    Shader _lightCullCompute;
    DepthFramebuffer _depthFBO;
    ForwardCompInitializers _forwardCompInits;

    void InitializeLightCull(uint32_t width, uint32_t height);
    void InitializeDepthBuffer(uint32_t width, uint32_t height);

    void DepthPrePass(AssetManager& manager) const;
    void LightCullPass();
public:
    void Initialize(uint32_t width, uint32_t height);
    void Render(AssetManager& manager);
};