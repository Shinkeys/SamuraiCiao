#include "../../headers/graphics/shadows.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/interface.h"


void ShadowsHelper::PassLanterns(LightSources* lant)
{
    _lightSources = lant;
}


void ShadowsHelper::Prepare()
{
    glDepthMask(GL_TRUE);
    // generating scene from light point of view
    glGenFramebuffers(1, &_frameBuffer.buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer.buffer);

    _shadowTexExtent.first = 2048u;
    _shadowTexExtent.second = 2048u;

    glGenTextures(1, &_frameBuffer.texture);
    glBindTexture(GL_TEXTURE_2D, _frameBuffer.texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _shadowTexExtent.first, 
        _shadowTexExtent.second, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


    constexpr std::array<float, 4> clampColor = {0.5f, 0.5f, 0.5f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // attaching texture to depth framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        _frameBuffer.texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer incomplete\n";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);   

    // Creating shader for depth passes
    Shader depthShader;
    depthShader.LoadShaders("depthPass.vert", "depthPass.frag");
    RenderManager::AddShaderByType(std::move(depthShader), RenderPassType::RENDER_DEPTHPASS);


    // adding shader to draw in main render pass
    TextureDesc depthTexDesc;
    depthTexDesc.name = "shadowsTexture";
    depthTexDesc.type = RenderPassType::RENDER_MAIN;
    depthTexDesc.handle = _frameBuffer.texture;
    depthTexDesc.bindSlot = 5;

    RenderManager::AttachTextureToDraw(depthTexDesc);

}

void ShadowsHelper::DrawDepthScene(AssetManager& manager)
{
    glBindVertexArray(manager.GetAssetsVAO());
    
    if(_lightSources != nullptr)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer.buffer);
        OpenglBackend::SetViewport(_shadowTexExtent.first, _shadowTexExtent.second);


        const glm::mat4 lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, _nearPlane, _farPlane);
        for(const auto& light : _lightSources->GetLightSources())
        {
            if (light.second.type != LightType::LIGHT_DIRECTIONAL)
                continue;

            glm::mat4 lightMatrix = glm::mat4(1.0f);
            const std::string lightMatrixName = "lightMatrix";

            if(light.second.type == LightType::LIGHT_DIRECTIONAL)
            {
                const glm::vec3 lightViewPoint = glm::vec3(0.0f, 150.0f, 355.0f);
                const glm::vec3 centerPointTemporary = glm::vec3(0.0f, 0.0f, -20.0f);
                const glm::mat4 lightView = glm::lookAt(lightViewPoint, centerPointTemporary, glm::vec3(0.0f, 1.0f, 0.0f));
                lightMatrix = lightProj * lightView;

            }
            // adding this matrix to draw in main pass too
            MatrixDesc matrixDesc;
            matrixDesc.data = std::move(lightMatrix);
            matrixDesc.name = lightMatrixName;
            matrixDesc.type = RenderPassType::RENDER_MAIN;
            RenderManager::AttachMatrixToBind(matrixDesc);

            RenderManager::DrawDepthPass(manager, matrixDesc.data);

        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else std::cout << "Lanterns object is nullptr\n";

}


void ShadowsHelper::DebugShadows()
{
    static bool shadows = false;
    ImGui::Checkbox("Shadows", &shadows);
    if(shadows)
    {  
       ImGui::Text("Depth texture:");
       // For flip
       ImVec2 uv0 = ImVec2(1.0f, 1.0f);
       ImVec2 uv1 = ImVec2(0.0f, 0.0f);
       ImGui::Image(_frameBuffer.texture, ImVec2{512.0f, 512.0f}, uv0, uv1);
    }
}

void ShadowsHelper::MakeDynamicArea(float zNear, float zFar)
{

}