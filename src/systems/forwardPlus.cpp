#include "../../headers/systems/forwardPlus.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"


void ForwardPlusRender::InitializeLightCull(uint32_t width, uint32_t height)
{
    _lightCullCompute.LoadComputeShader("lightCull.comp");
    _lightCullCompute.UseShader();

    // Data to dispatch comp shader
    _forwardCompInits.numGroupsX = std::ceil(+(width  / _forwardCompInits.tileSize));
    _forwardCompInits.numGroupsY = std::ceil(+(height / _forwardCompInits.tileSize));
}

void ForwardPlusRender::InitializeDepthBuffer(uint32_t width, uint32_t height)
{
    // generating scene from light point of view
    glGenFramebuffers(1, &_depthFBO.buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO.buffer);

    glGenTextures(1, &_depthFBO.texture);
    glBindTexture(GL_TEXTURE_2D, _depthFBO.texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, 
        height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


    const std::array<float, 4> clampColor = {1.0f, 1.0f, 1.0f, 1.0f}; 
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

void ForwardPlusRender::Initialize(uint32_t width, uint32_t height)
{
    InitializeDepthBuffer(width, height);
    InitializeLightCull(width, height);
}


void ForwardPlusRender::Render(AssetManager& manager)
{
    glBindVertexArray(manager.GetAssetsVAO());

    DepthPrePass(manager);
    LightCullPass();

    // Main renderer
    RenderManager::DrawSkybox(manager);
    RenderManager::DrawMainScene(manager);
}

void ForwardPlusRender::LightCullPass()
{
    _lightCullCompute.UseShader();


    glDispatchCompute(_forwardCompInits.numGroupsX, _forwardCompInits.numGroupsY, 1);
    // glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ForwardPlusRender::DepthPrePass(AssetManager& manager) const
{
    // Depth pass
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO.buffer);
    const auto& matrices = SamuraiCameras::g_activeCamera->GetMVP();    
    RenderManager::DrawDepthPass(manager, matrices.projection * matrices.view);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}