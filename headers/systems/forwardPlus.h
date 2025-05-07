#pragma once
#include "../types/renderTypes.h"
#include "../types/types.h"
#include "../types/openglTypes.h"
#include "shaders.h"


struct FrustumComputeInitializers
{
    int32_t tileSize = 16;
    int32_t numGroupsX = 128;
    int32_t numGroupsY = 128;
};

struct ViewFrustumDesc
{
    std::array<glm::vec4, 4> normals;
    std::array<float, 4> distToPlane;
};

struct FrustumSSBOHandle
{
    uint32_t id;
    std::vector<ViewFrustumDesc> data;
};


struct LightCullComputeInitializers
{
    const int32_t maxLights = 512;
    // int32_t numGroupsX = 128;
    // int32_t numGroupsY = 128;
};

struct LightDesc
{
    glm::vec3 position;
    glm::vec3 color;
    float radius;
};

struct LightCullSSBOHandle
{
    uint32_t id;
    std::vector<LightDesc> data;
};

class AssetManager;
class ForwardPlusRender
{
private:
    Shader _frustumCompute;
    Shader _lightCullCompute;
    DepthFramebuffer _depthFBO;
    FrustumComputeInitializers _frustumComputeInitializers;

    FrustumSSBOHandle _frustumSSBO;
    LightCullSSBOHandle _lightCullSSBO;
    LightCullComputeInitializers _lightCullComputeInitializers;

    void InitializeFrustumCull(uint32_t width, uint32_t height);
    void InitializeLightCull();
    void InitializeDepthBuffer(uint32_t width, uint32_t height);

    void DepthPrePass(AssetManager& manager) const;
    void LightCullPass();
public:
    void Initialize(uint32_t width, uint32_t height);
    void Render(AssetManager& manager);
};