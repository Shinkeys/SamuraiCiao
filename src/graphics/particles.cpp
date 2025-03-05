#include "../../headers/graphics/particles.h"
#include "../../headers/systems/shaders.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/types/random.h"
#include "../../headers/systems/interface.h"


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
    // model = glm::translate(model, glm::vec3(-50.0f, 5.0f, -100.0f));
    // model = glm::scale(model, glm::vec3(50.0f));
    mainShader.SetMat4x4("model", model);

    RenderManager::AddShaderByType(std::move(computeShader), RenderPassType::RENDER_PARTICLES_COMP);
    RenderManager::AddShaderByType(std::move(mainShader), RenderPassType::RENDER_PARTICLES);

    return true;
}

// Purpose: change positions of particles randomly
void Particles::RandomDistribution()
{
    glm::vec3 particlesDisplacement;
    particlesDisplacement.x = Random::RandomFloat(-0.010f, 0.010f);
    particlesDisplacement.y = Random::RandomFloat(-0.009f, 0.0f);
    particlesDisplacement.z = Random::RandomFloat(-0.008f, 0.015f);

    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_PARTICLES_COMP);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        shaderIt->second.SetVec3("displacement", particlesDisplacement);
    }
    else
        std::cout << "Can't update particles position, shader not  found\n";
}

void Particles::FillParticlesPoints()
{
    // setup vertices is basically POINTS(their placement)
    _setup.vertices.reserve(_numberOfParticles);


    float offsetX = -250.0f;
    float offsetZ = -150.0f;
    for(uint32_t i = 0; i < _numberOfParticles; ++i)
    {   
        float bias = Random::RandomFloat(-15.0f, 15.0f);
        offsetX += 25.5f + bias;
        if(i % 25 == 0)
        {
            offsetX = -250.0f;
            offsetZ += static_cast<float>(i);
        }
        
        const float xPos = offsetX;
        const float yPos = 75.0f * std::clamp(std::abs(bias), 0.9f, 2.0f);
        const float zPos = offsetZ * (bias / 15.0f);


        _setup.vertices.push_back(glm::vec4(xPos, yPos, zPos, 1.0f));
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
    if(_particlesEnabled)
    {
        ExecuteCompShader();
        // important: to be here as in previous function used needed comp. shader
        RandomDistribution();

        DrawParticles();

    }
}

void Particles::EnableParticles()
{
    ImGui::Checkbox("Particles", &_particlesEnabled);

    
    static int32_t selected = 0;
    
    float posOffset = SamuraiInterface::g_windowWidth / 1.75f;
    ImGui::SameLine(posOffset);
    if(ImGui::RadioButton("256", selected == 0))
    {
        selected = 0;
        _numberOfParticles = 256;
    }
    posOffset += posOffset / 3.0f;
    ImGui::SameLine(posOffset);
    if(ImGui::RadioButton("512", selected == 1))
    {
        selected = 1;
        _numberOfParticles = 512;
    }
    posOffset += posOffset / 3.0f;
    ImGui::SameLine(posOffset);
    if(ImGui::RadioButton("1024", selected == 2))
    {
        selected = 2;
        _numberOfParticles = 1024;
    }
}

void Particles::DrawParticles()
{
    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_PARTICLES);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        shaderIt->second.UseShader();
        shaderIt->second.SetMat4x4("view", Camera::GetMVP().view);
        shaderIt->second.SetMat4x4("projection", Camera::GetMVP().projection);

        // variable with random number to shuffle colors
        const float randomValue = Random::RandomFloat(-2.0f, 2.0f);
        shaderIt->second.SetFloat("random", randomValue);
    }
    glBindVertexArray(_setup.VAO);
    glDrawArrays(GL_POINTS, 0, _numberOfParticles);
    glBindVertexArray(0);
}

void Particles::ExecuteCompShader()
{  
    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_PARTICLES_COMP);
    if(shaderIt != RenderManager::_shaderTypes.end())
    {
        shaderIt->second.UseShader();
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _ssboId);

        // work group size
        glDispatchCompute(_numberOfParticles, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        
        const int32_t totalDataSize = sizeof(glm::vec4) * _numberOfParticles;
        // passing data to the vbo
        glCopyNamedBufferSubData(_ssboId, _setup.VBO, 0, 0, totalDataSize);
    }

}