#pragma once
#include "../types/renderTypes.h"
#include "../types/types.h"
#include "../types/openglTypes.h"
#include "shaders.h"
#include "../graphics/lightSources.h"

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

struct LightCullInitializers
{
    uint32_t lightGridTexHandle = 0;
    uint32_t lightGridTexWidth  = 0;
    uint32_t lightGridTexHeight = 0;
    uint32_t lightGridBindId    = 0;

    uint32_t maxLights = 0;
};

struct LightDescCompute
{
    glm::vec3 positionView;
    float radius;
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

    LightCullInitializers _lightCullInitializers;


    // For compute shader, light cull pass. Only point lights counts.
    std::vector<LightDescCompute> _pointLightsDescForCompute;

    void InitializeFrustumCull(uint32_t width, uint32_t height);
    void InitializeLightCull();
    void InitializeDepthBuffer(uint32_t width, uint32_t height);

    void DepthPrePass(AssetManager& manager)                const;
    void LightCullPass();

public:
    void Initialize(uint32_t width, uint32_t height);
    void Update(const Window& window) const;
    void Render(AssetManager& manager)                      const;
};