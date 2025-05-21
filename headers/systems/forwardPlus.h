#pragma once
#include "../types/renderTypes.h"
#include "../types/lightTypes.h"
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
struct LightCullInitializers
{
    uint32_t numGroupsX = 16;
    uint32_t numGroupsY = 16;

    uint32_t lightGridTexHandle = 0;
    uint32_t lightGridTexWidth  = 0;
    uint32_t lightGridTexHeight = 0;
    uint32_t lightGridBindId    = 0;

};

struct LightDescCompute
{
    glm::vec3 positionView;
    float radius;
};

struct LightIndexList
{
    uint32_t lightIndicesBindID;
    uint32_t lightIndicesHandle;
    std::vector<int32_t> lightIndices;

    uint32_t globalIndexBindID;
    uint32_t globalIndexHandle;
};

class AssetManager;
class ForwardPlusRender
{
private:
    LightSources* _lightSources = nullptr;


    Shader _lightCullCompute;
    DepthFramebuffer _depthFBO;

    LightCullInitializers _lightCullInitializers;
    LightIndexList _lightIndexList;

    // For compute shader, light cull pass. Only point lights counts.
    std::vector<LightDescCompute> _pointLightsDescForCompute;

    void InitializeLightCull(uint32_t width, uint32_t height);
    void InitializeDepthBuffer(uint32_t width, uint32_t height);

    void DepthPrePass(AssetManager& manager)                const;
    void LightCullPass();
    void SetUniformsForRender(Shader& shader) const;
public:
    void PassLightSources(LightSources& lightSources);
    void Initialize(uint32_t width, uint32_t height);
    void Update(const Window& window) const;
    void Render(AssetManager& manager);
};