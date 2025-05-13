#include "../../headers/systems/forwardPlus.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/backend/openglbackend.h"


// void ForwardPlusRender::GatherLightsInfo(const LightSources& lightsInstance)
// {
//     const auto& lights = lightsInstance.GetLightSources();
//
//     for (const auto& light : lights)
//     {
//         if (light.type == LightDescriptor::LightType::LIGHT_POINT)
//         {
//             LightDescCompute   lightComp;
//             lightComp.positionView = glm::mat3(SamuraiCameras::g_activeCamera->GetMVP().view) * light.data;
//
//             glm::vec3 lightLuminance = glm::dot(light.color, )
//
//             lightComp.radius =
//
//             _pointLightsDescForCompute.push_back(light);
//         }
//
//
//     }
// }

void ForwardPlusRender::InitializeFrustumCull(uint32_t width, uint32_t height)
{
    _frustumCompute.LoadComputeShader("forwardPlus/frustum.comp");
    // Data to dispatch comp shader
    // Split screen into 16x16(pixels) tiles
    _frustumComputeInitializers.numGroupsX = std::ceil(+(width  / _frustumComputeInitializers.tileSize));
    _frustumComputeInitializers.numGroupsY = std::ceil(+(height / _frustumComputeInitializers.tileSize));


    // Init SSBO
    const int32_t tilesCount =  _frustumComputeInitializers.numGroupsX * _frustumComputeInitializers.numGroupsY;
    constexpr uint32_t viewFrustumBindNum = 1;
    SSBOBind<ViewFrustumDesc> bindData;
    bindData.binding = &viewFrustumBindNum;
    bindData.ssboId = &_frustumSSBO.id;
    bindData.data = nullptr;
    bindData.size = sizeof(ViewFrustumDesc) * tilesCount;
    bindData.type = GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT;
    if(OpenglBackend::CreateSSBOImmutable(bindData) == ErrorCodes_Backend::ERROR_SSBO_CREATION)
    {
        std::cout << "Unable to initialize light cull, ssbo is not created\n";
        return;
    }


    _frustumCompute.UseShader();
    _frustumCompute.SetVec2("screenResolution", glm::vec2(static_cast<float>(width), static_cast<float>(height)));
    _frustumCompute.SetVec3("eyePos", SamuraiCameras::g_activeCamera->GetPosition());
        
    const glm::mat4& proj = SamuraiCameras::g_activeCamera->GetMVP().projection;
    const glm::mat4 inverseProj = glm::inverse(proj);
    _frustumCompute.SetMat4x4("inverseProjection", inverseProj);

    glDispatchCompute(_frustumComputeInitializers.numGroupsX, _frustumComputeInitializers.numGroupsY, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void ForwardPlusRender::InitializeLightCull()
{
    _lightCullCompute.LoadComputeShader("forwardPlus/lightCull.comp");

    _lightCullInitializers.lightGridTexWidth = _frustumComputeInitializers.numGroupsX;
    _lightCullInitializers.lightGridTexHeight = _frustumComputeInitializers.numGroupsY;
    _lightCullInitializers.lightGridBindId = 4;
    _lightCullInitializers.maxLights = 1024;

    // Init SSBO
    constexpr uint32_t lightCullBindNum = 1;
    SSBOBind<LightDesc> bindData;
    bindData.binding = &lightCullBindNum;
    bindData.ssboId = &_frustumSSBO.id;
    bindData.data = nullptr;
    bindData.size = sizeof(LightDesc) * _lightCullInitializers.maxLights;
    bindData.type = GL_MAP_PERSISTENT_BIT | GL_MAP_READ_BIT;
    if(OpenglBackend::CreateSSBOImmutable(bindData) == ErrorCodes_Backend::ERROR_SSBO_CREATION)
    {
        std::cout << "Unable to initialize light cull, ssbo is not created\n";
        return;
    }

    // Lighting grid should be represented as 2d texture

    glGenTextures(1,  &_lightCullInitializers.lightGridTexHandle);
    glBindTexture(GL_TEXTURE_2D, _lightCullInitializers.lightGridTexHandle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, static_cast<GLsizei>(_lightCullInitializers.lightGridTexWidth),
        static_cast<GLsizei>(_lightCullInitializers.lightGridTexHeight), 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_2D, 0);


    TextureDesc texDesc;
    texDesc.name = "lightsGrid";
    texDesc.handle = _lightCullInitializers.lightGridTexHandle;
    texDesc.type = RenderPassType::RENDER_MAIN;
    texDesc.bindSlot = 6;
    RenderManager::AttachTextureToDraw(texDesc);

}

void ForwardPlusRender::Initialize(uint32_t width, uint32_t height)
{
    InitializeDepthBuffer(width, height);
    InitializeFrustumCull(width, height);
    InitializeLightCull();
}


void ForwardPlusRender::Render(AssetManager& manager) const
{
    glBindVertexArray(manager.GetAssetsVAO());

    DepthPrePass(manager);
    // LightCullPass();


    // Setting variables to the shader
    // RenderManager::AttachVectorToBind()

    // Main renderer
    RenderManager::DrawSkybox(manager);

    // Don't care about overhead now, it's less than 2 nanoseconds for the usage of the same shader twice in a row
    auto shaderMainIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_MAIN);
    if (shaderMainIt != RenderManager::_shaderTypes.end())
    {

        shaderMainIt->second.UseShader();
        shaderMainIt->second.Set
    }
    else
    {
        std::cout << "Shader for forward plus render binding is not found\n";
    }
    RenderManager::DrawMainScene(manager);
}

void ForwardPlusRender::LightCullPass()
{
    _lightCullCompute.UseShader();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _lightCullSSBO.id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _frustumSSBO.id);

    glBindImageTexture(_lightCullInitializers.lightGridBindId, GL_TEXTURE_2D, 0,  GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32UI);

    int32_t lightsCounterUniform = 0;
    _lightCullCompute.SetInt("lightsCounter", lightsCounterUniform);


    // ??? to do
    glDispatchCompute(_frustumComputeInitializers.numGroupsX, _frustumComputeInitializers.numGroupsY, 1);
    

}

void ForwardPlusRender::DepthPrePass(AssetManager& manager) const
{
    // Depth pass
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO.buffer);
    const auto& matrices = SamuraiCameras::g_activeCamera->GetMVP();    
    RenderManager::DrawDepthPass(manager, matrices.projection * matrices.view);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardPlusRender::Update(const Window& window) const
{
    // if (window.)
}



void ForwardPlusRender::InitializeDepthBuffer(uint32_t width, uint32_t height)
{
    // generating scene from light point of view
    glGenFramebuffers(1, &_depthFBO.buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO.buffer);

    glGenTextures(1, &_depthFBO.texture);
    glBindTexture(GL_TEXTURE_2D, _depthFBO.texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<GLsizei>(width),
        static_cast<GLsizei>(height), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


    constexpr std::array<float, 4> clampColor = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // attaching texture to depth framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        _depthFBO.texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);   
}