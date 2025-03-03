#include "../../headers/graphics/particles.h"
#include "../../headers/systems/shaders.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"

void Particles::GenerateParticles()
{
    glEnable(GL_PROGRAM_POINT_SIZE);

    // filling path with current positions of particles
    FillParticlesPoints();
    
    Shader computeShader;
    computeShader.LoadComputeShader("particles.comp");
    computeShader.UseShader();

    // ssbo
    const uint32_t verticesBindNumber = 1;
    SSBOBindVec4 bindData;
    bindData.binding = &verticesBindNumber;
    bindData.ssboId = &_ssboId;
    bindData.data = _setup.vertices.data();
    bindData.size = sizeof(_setup.vertices[0]) * _setup.vertices.size();
    OpenglBackend::CreateSSBO(bindData);

    // vertex/frag shaders
    Shader mainShader;
    mainShader.LoadShaders("particles.vert", "particles.frag");
    mainShader.UseShader();
    

    OpenglBackend::BindModelVBO(_setup);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-35.0f, 5.0f, 35.0f));
    // model = glm::scale(model, glm::vec3(50.0f));
    mainShader.SetMat4x4("model", model);

    RenderManager::AddShaderByType(std::move(computeShader), RenderPassType::RENDER_PARTICLES_COMP);
    RenderManager::AddShaderByType(std::move(mainShader), RenderPassType::RENDER_PARTICLES);
}

void Particles::FillParticlesPoints()
{
    // setup vertices is basically POINTS(their placement)
    _setup.vertices.reserve(_numberOfParticles);

    // offset of positions
    float offsetX = 0.0f;
    float offsetZ = 0.0f;
    for(uint32_t i = 0; i < _numberOfParticles; ++i)
    {   
        offsetX += 3.5f;
        if(i % 25 == 0)
        {
            offsetX = 0.0f;
            offsetZ += static_cast<float>(i) * 0.05f;
        }
        _setup.vertices.push_back(glm::vec4(offsetX, 0.0f, offsetZ, 1.0f));
    }

}

void Particles::DrawParticles()
{
    glBindVertexArray(_setup.VAO);
    ExecuteCompShader();
    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_PARTICLES);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        shaderIt->second.UseShader();
        shaderIt->second.SetMat4x4("view", Camera::GetMVP().view);
        shaderIt->second.SetMat4x4("projection", Camera::GetMVP().projection);
    }

    glDrawArrays(GL_POINTS, 0, _setup.vertices.size());
    glBindVertexArray(0);
}

void Particles::ExecuteCompShader()
{  
    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_PARTICLES_COMP);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        shaderIt->second.UseShader();
        glDispatchCompute(_numberOfParticles, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

}