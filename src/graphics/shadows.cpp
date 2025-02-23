#include "../../headers/graphics/shadows.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"


void ShadowsHelper::Prepare()
{
    // generating scene from light point of view
    glGenFramebuffers(1, &_depthFBO);
    
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
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
    _depthTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   
}

void ShadowsHelper::DrawDepthScene(AssetManager& manager, Shader& shader)
{
    OpenglBackend::SetViewport(_shadowTexExtent.first, _shadowTexExtent.second);
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    shader.UseShader();
    manager.BindStructures();

    const Matrices* matrices = &Camera::GetMVP();
    if(matrices != nullptr)
    {
        shader.SetMat4x4("view", matrices->view);
        shader.SetMat4x4("projection", matrices->projection);
    }
    else
    {
        std::cout << "Unable to find matrices for current mesh\n";
    }

    float near = 0.1f;
    float far = 100.0f;
    glm::mat4 lightProj = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near, far);
    for(const auto& lightSrc : manager.GetLightSources())
    {
        glm::mat4 lightView = glm::lookAt(lightSrc.second, 
        glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 lightSpaceMatrix = lightProj * lightView;

    }


    for(const auto& mesh : manager.GetAssetStorage())
    {
        glm::mat4 modelMat = manager.GetTransformMatrix().find(mesh.first)->second;
        shader.SetMat4x4(mesh.first, modelMat);
        manager.DrawParticularModel(mesh.first);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}