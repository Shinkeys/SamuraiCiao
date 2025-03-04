#include "../../headers/graphics/particles.h"
#include "../../headers/systems/shaders.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"

bool Particles::GenerateParticles()
{
    glEnable(GL_PROGRAM_POINT_SIZE);

    // filling path with current positions of particles
    FillParticlesPoints();
    
    _setup.type = GL_DYNAMIC_COPY | GL_DYNAMIC_DRAW;
    OpenglBackend::BindModelVBO(_setup);

    Shader computeShader;
    computeShader.LoadComputeShader("particles.comp");
    computeShader.UseShader();

    // ssbo
    if(_setup.vertices.size() == 0)
    {
        std::cout << "Can't generate particles as vertices vector is empty!";
        return false;
    }

    const uint32_t verticesBindNumber = 1;
    SSBOBindVec4 bindData;
    bindData.binding = &verticesBindNumber;
    bindData.ssboId = &_ssboId;
    bindData.data = _setup.vertices.data();
    bindData.size = sizeof(glm::vec4) * _setup.vertices.size();
    bindData.type = GL_DYNAMIC_COPY;
    if(OpenglBackend::CreateSSBO(bindData) == ErrorCodes_Backend::ERROR_SSBO_CREATION)
    {
        std::cout << "Unable to initialize particles\n";
        return false;
    }

    // vertex/frag shaders
    Shader mainShader;
    mainShader.LoadShaders("particles.vert", "particles.frag");
    mainShader.UseShader();
    
    
    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(-35.0f, 5.0f, 35.0f));
    // model = glm::scale(model, glm::vec3(50.0f));
    mainShader.SetMat4x4("model", model);

    RenderManager::AddShaderByType(std::move(computeShader), RenderPassType::RENDER_PARTICLES_COMP);
    RenderManager::AddShaderByType(std::move(mainShader), RenderPassType::RENDER_PARTICLES);


    return true;
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

void Particles::Prepare()
{
    if(!GenerateParticles())
    {
        std::cout << "Unable to generate particles\n";
        return;
    }

    // to do: imgui
}

void Particles::RenderParticles()
{
    ExecuteCompShader();

    DrawParticles();
}

void Particles::DrawParticles()
{
    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_PARTICLES);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        shaderIt->second.UseShader();
        shaderIt->second.SetMat4x4("view", Camera::GetMVP().view);
        shaderIt->second.SetMat4x4("projection", Camera::GetMVP().projection);
    }
    glBindVertexArray(_setup.VAO);
    glDrawArrays(GL_POINTS, 0, _setup.vertices.size());
    glBindVertexArray(0);
}

void Particles::ExecuteCompShader()
{  
    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_PARTICLES_COMP);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        shaderIt->second.UseShader();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _ssboId);
        glDispatchCompute(_numberOfParticles, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // passing data to the vbo
        glCopyNamedBufferSubData(_ssboId, _setup.VBO, 0, 0, _numberOfParticles);
    }

}