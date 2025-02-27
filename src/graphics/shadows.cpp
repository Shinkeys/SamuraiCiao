#include "../../headers/graphics/shadows.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/interface.h"


void ShadowsHelper::Prepare()
{
    glDepthMask(GL_TRUE);
    // generating scene from light point of view
    glGenFramebuffers(1, &_depthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);
    
    _shadowTexExtent.first = 2048u;
    _shadowTexExtent.second = 2048u;

    glGenTextures(1, &_depthTex);
    glBindTexture(GL_TEXTURE_2D, _depthTex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _shadowTexExtent.first, 
        _shadowTexExtent.second, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // attaching texture to depth framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
    _depthTex, 0);
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
    depthTexDesc.handle = _depthTex;

    const auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_MAIN);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        depthTexDesc.shader = &shaderIt->second;
    }
    else
    {
        std::cout << "Shader for shadows not found\n";
    }
    RenderManager::AttachTextureToDraw(depthTexDesc);

}

void ShadowsHelper::DrawDepthScene(AssetManager& manager)
{
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);
    OpenglBackend::SetViewport(_shadowTexExtent.first, _shadowTexExtent.second);
    glClear(GL_DEPTH_BUFFER_BIT);


    // getting shader where shadows are calculated
    auto shader = RenderManager::_shaderTypes.find(RenderPassType::RENDER_DEPTHPASS);

    if(shader == RenderManager::_shaderTypes.end())
    {
        std::cout << "Shader for shadows not found\n";
        return;
    }

    shader->second.UseShader();
    const Matrices* matrices = &Camera::GetMVP();


    // to rework
    float near = 0.01f;
    float far = 20.0f;
    glm::mat4 lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near, far);
    glm::mat4 lightMatrix = glm::mat4(1.0f);
    const std::string lightMatrixName = "lightMatrix";
    for(const auto& lightSrc : manager.GetLightSources())
    {
        glm::mat4 lightView = glm::lookAt(lightSrc.second, 
        glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightMatrix = lightProj * lightView;

        shader->second.SetMat4x4(lightMatrixName, lightMatrix);        
    }   
    // adding this matrix to draw in main pass too
    MatrixDesc matrixDesc;
    matrixDesc.data = std::move(lightMatrix);
    matrixDesc.name = lightMatrixName;
    matrixDesc.shader = &shader->second;
    matrixDesc.type = RenderPassType::RENDER_MAIN;
    RenderManager::AttachMatrixToBind(matrixDesc);


    for(const auto& mesh : manager.GetAssetStorage())
    {
        const glm::mat4* modelMat = manager.GetTransformMatrixByName(mesh.second.modelName);
        if(modelMat != nullptr)
            shader->second.SetMat4x4("model", *modelMat);
        else std::cout << "Model matrix for shadows not found\n";
        

        // actually drawing
        for(uint32_t i = 0; i < mesh.second.currMeshVertCount.size(); ++i)
        {
            const uint32_t vertexCount = mesh.second.currMeshVertCount[i];
            const uint32_t offset = mesh.second.meshIndexOffset[i];
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                (void*)(offset + manager.GetBuffers().indices.data()));
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_CULL_FACE);
}


void ShadowsHelper::DebugShadows()
{
    static bool shadows = false;
    ImGui::Checkbox("Shadows", &shadows);
    if(shadows)
    {  
       ImGui::Text("Depth texture:");
       ImGui::Image(_depthTex, ImVec2{256.0f, 256.0f}); 
    }
}