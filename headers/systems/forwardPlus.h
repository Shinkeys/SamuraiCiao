#pragma once
#include "../types/renderTypes.h"
#include "../types/lightTypes.h"
#include "../types/openglTypes.h"
#include "shaders.h"
#include "../graphics/lightSources.h"
#include "../backend/openglbackend.h"


struct ViewFrustumDesc
{
    std::array<glm::vec4, 4> normals;
    std::array<float, 4> distToPlane;
};
struct LightCullInitializers
{
    uint32_t numGroupsX = 16;
    uint32_t numGroupsY = 16;

    uint32_t lightGridTexHandle{ 0 };
    uint32_t lightGridTexWidth{ 0 };
    uint32_t lightGridTexHeight{ 0 };
    uint32_t lightGridBindId{ 0 };

};

struct LightDescCompute
{
    glm::vec3 positionView;
    float radius;
};

struct LightIndexList
{
    uint32_t lightIndicesBindID{ 0 };
    uint32_t lightIndicesHandle{ 0 };
    std::vector<int32_t> lightIndices;

    uint32_t globalIndexBindID{ 0 };
    uint32_t globalIndexHandle{ 0 };
};

struct DebugRenderDesc
{
    Shader shader;
    
    bool enabled{ false };
    // Rectangle to draw on some texture/colors on top of it(layer above of the current image)
    EBOSetupUnskinned rectangle;
};

class AssetManager;
class ForwardPlusRender
{
private:
    LightSources* _lightSources = nullptr;

    bool _drawShadows{ true };

    Shader _lightCullCompute;

    DebugRenderDesc _debugRenderInstance;
    DepthFramebuffer _depthFBO;

    LightCullInitializers _lightCullInitializers;
    LightIndexList _lightIndexList;

    // For compute shader, light cull pass. Only point lights counts.
    std::vector<LightDescCompute> _pointLightsDescForCompute;

    void InitializeLightCull(uint32_t width, uint32_t height);
    void InitializeDepthBuffer(uint32_t width, uint32_t height);
    void InitializeDebugRender();

    void DepthPrePass(AssetManager& manager)                const;
    void LightCullPass();
    void DebugRenderPass();
    void SetUniformsForRender(Shader& shader) const;
public:
    void HandleInterface();
    void PassLightSources(LightSources& lightSources);
    void Initialize(uint32_t width, uint32_t height);
    void Update(const Window* window);
    void Render(AssetManager& manager);
};