#include "../../headers/systems/forwardPlus.h"
#include "../../headers/systems/camera.h"
#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/interface.h"


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


void ForwardPlusRender::PassLightSources(LightSources& lightSources)
{
    _lightSources = &lightSources;
}

void ForwardPlusRender::InitializeLightCull(uint32_t width, uint32_t height)
{
    _lightCullCompute.LoadComputeShader("forwardPlus/lightCull.comp");

    _lightCullInitializers.numGroupsX = std::ceil(+(width / 16));
    _lightCullInitializers.numGroupsY = std::ceil(+(height / 16));

    size_t numberOfTiles = _lightCullInitializers.numGroupsX * _lightCullInitializers.numGroupsY;

    _lightCullInitializers.lightGridTexWidth  = _lightCullInitializers.numGroupsX;
    _lightCullInitializers.lightGridTexHeight = _lightCullInitializers.numGroupsY;
    _lightCullInitializers.lightGridBindId = 3;

    // Buffer light indices
    constexpr uint32_t globalLightIndicesBindNum = 2;
    SSBOBind<int32_t> indicesBindData;
    _lightIndexList.lightIndicesBindID = 2;
    indicesBindData.binding = &_lightIndexList.lightIndicesBindID;
    indicesBindData.ssboId  = &_lightIndexList.lightIndicesHandle;
    int32_t tilesPerScreen = static_cast<int32_t>(_lightCullInitializers.numGroupsX * _lightCullInitializers.numGroupsY);
    _lightIndexList.lightIndices.insert(_lightIndexList.lightIndices.end(), tilesPerScreen * LightDefines::g_max_lights_per_tile, -1);
    indicesBindData.data    = _lightIndexList.lightIndices.data();
    indicesBindData.size    = sizeof(int32_t) * tilesPerScreen * LightDefines::g_max_lights_per_tile;
    indicesBindData.type    = GL_DYNAMIC_STORAGE_BIT;
    if(OpenglBackend::CreateSSBOImmutable(indicesBindData) == ErrorCodes_Backend::ERROR_SSBO_CREATION)
    {
        std::cout << "Unable to initialize lights indices buffer, SSBO is not created\n";
    }


    // Buffer for counter into lightIndices buff
    SSBOBind<uint32_t> globalIndexBindData;
    _lightIndexList.globalIndexBindID = 5;
    globalIndexBindData.binding = &_lightIndexList.globalIndexBindID;
    globalIndexBindData.ssboId  = &_lightIndexList.globalIndexHandle;
    globalIndexBindData.data    = nullptr;
    globalIndexBindData.size    = sizeof(uint32_t);
    globalIndexBindData.type    = GL_DYNAMIC_STORAGE_BIT;
    if(OpenglBackend::CreateSSBOImmutable(globalIndexBindData) == ErrorCodes_Backend::ERROR_SSBO_CREATION)
    {
        std::cout << "Unable to initialize lights indices buffer, SSBO is not created\n";
    }

    // Lighting grid should be represented as 2d texture
    glGenTextures(1,  &_lightCullInitializers.lightGridTexHandle);
    glBindTexture(GL_TEXTURE_2D, _lightCullInitializers.lightGridTexHandle);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32UI, _lightCullInitializers.lightGridTexWidth, _lightCullInitializers.lightGridTexHeight);


    glBindTexture(GL_TEXTURE_2D, 0);


}

void ForwardPlusRender::InitializeDebugRender()
{
    _debugRenderInstance.shader.LoadShaders("forwardPlus/debug.vert", "forwardPlus/debug.frag");


    // Fill rectangle
    auto& rectangle = _debugRenderInstance.rectangle.vertices;
    // left bottom
    rectangle.push_back(glm::vec3(-1.0f, -1.0f, 0.0f));
    // right bottom
    rectangle.push_back(glm::vec3(1.0f, -1.0f, 0.0f));
    // left top
    rectangle.push_back(glm::vec3(-1.0f, 1.0f, 0.0f));
    // right top
    rectangle.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

    auto& indices = _debugRenderInstance.rectangle.indices;
    indices.insert(indices.end(), { 0, 1, 3, 3, 2, 0});

    auto createBuffersRes = OpenglBackend::BindModelEBO(_debugRenderInstance.rectangle);

    if (createBuffersRes != ErrorCodes_Backend::NO_ERROR)
    {
        std::cout << "Error in buffer creation for debug instance\n";
    }

}

void ForwardPlusRender::Initialize(uint32_t width, uint32_t height)
{
    InitializeDepthBuffer(width, height);
    InitializeLightCull(width, height);
    InitializeDebugRender();
}
void ForwardPlusRender::SetUniformsForRender(Shader& shader) const
{
    if (_drawShadows)
    {
        glm::vec3 directionalLightDir = _lightSources->GetDirectionalLightDir();
        shader.SetVec3("directionalLightDir", directionalLightDir);
    }
    glm::vec3 cameraPosition = SamuraiCameras::g_activeCamera->GetPosition();
    shader.SetVec3("cameraPosition", cameraPosition);
    // TEMP
    shader.SetIVec2("screenDimension", glm::ivec2(2560, 1440));
    shader.SetBool("drawShadows", _drawShadows);

    // binding lights grid for the main render pass
    glBindImageTexture(6, _lightCullInitializers.lightGridTexHandle, 0,  GL_FALSE, 0, GL_READ_ONLY, GL_RG32UI);
    // binding SSBO with lights data for the main render pass
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, _lightSources->GetLightBuffersHandle().lightsHandle);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, _lightIndexList.lightIndicesHandle);


}

void ForwardPlusRender::DebugRenderPass()
{
    if (_debugRenderInstance.enabled)
    {
        glBindVertexArray(_debugRenderInstance.rectangle.VAO);
        _debugRenderInstance.shader.UseShader();
        glBindImageTexture(1, _lightCullInitializers.lightGridTexHandle, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RG32UI);
        _debugRenderInstance.shader.SetInt("maxLightsPerCluster", LightDefines::g_max_lights_per_tile);


        const size_t elementsCount = _debugRenderInstance.rectangle.indices.size();
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, _debugRenderInstance.rectangle.indices.data());

    }
}

void ForwardPlusRender::Render(AssetManager& manager)
{
    if (_lightSources == nullptr)
    {
        std::cout << "You forgot to pass dependency of lights to the render class\n";
        return;
    }

    glBindVertexArray(manager.GetAssetsVAO());

    if (!_drawShadows)
    {
        DepthPrePass(manager);
        LightCullPass();
    }


    // Setting variables to the shader
    // RenderManager::AttachVectorToBind()

    // Main renderer
    RenderManager::DrawSkybox(manager);

    // Don't care about overhead now, it's less than 2 nanoseconds for the usage of the same shader twice in a row
    auto shaderMainIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_MAIN);
    if (shaderMainIt != RenderManager::_shaderTypes.end())
    {
        auto& shader = shaderMainIt->second;
        shader.UseShader();
        SetUniformsForRender(shader);
    }
    else
    {
        std::cout << "Shader for forward plus render binding is not found\n";
    }
    RenderManager::DrawMainScene(manager);


    DebugRenderPass();

}

void ForwardPlusRender::DepthPrePass(AssetManager& manager) const
{
    // Depth pass
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO.buffer);
    const auto& matrices = SamuraiCameras::g_activeCamera->GetMVP();    
    RenderManager::DrawDepthPass(manager, matrices.projection * matrices.view);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ForwardPlusRender::HandleInterface()
{
    ImGui::Checkbox("Shadows", &_drawShadows);
}

void ForwardPlusRender::Update(const Window* window)
{
    if (window == nullptr)
    {
        std::cout << "Can't handle command, window is null\n";
        return;
    }

    if (window->GetKeysState().debugScene)
    {
        _debugRenderInstance.enabled = true;
    }
    else
    {
        _debugRenderInstance.enabled = false;
    }
}


void ForwardPlusRender::InitializeDepthBuffer(uint32_t width, uint32_t height)
{
    glGenFramebuffers(1, &_depthFBO.buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO.buffer);

    glGenTextures(1, &_depthFBO.texture);
    glBindTexture(GL_TEXTURE_2D, _depthFBO.texture);


    _depthFBO.size.x = static_cast<float>(width);
    _depthFBO.size.y = static_cast<float>(height);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, static_cast<GLsizei>(_depthFBO.size.x),
        static_cast<GLsizei>(_depthFBO.size.y), 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);


    constexpr std::array<float, 4> clampColor = {1.0f, 1.0f, 1.0f, 0.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

void ForwardPlusRender::LightCullPass()
{
    _lightCullCompute.UseShader();
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _lightSources->GetLightBuffersHandle().lightsHandle);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _lightIndexList.lightIndicesBindID, _lightIndexList.lightIndicesHandle);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, _lightIndexList.globalIndexBindID, _lightIndexList.globalIndexHandle);
    glBindImageTexture(_lightCullInitializers.lightGridBindId, _lightCullInitializers.lightGridTexHandle, 0,  GL_FALSE, 0, GL_WRITE_ONLY, GL_RG32UI);


    glBindTextureUnit(4, _depthFBO.texture);
    const uint32_t lightsCounterUniform = static_cast<uint32_t>(_lightSources->GetLightSources().size());
    _lightCullCompute.SetUInt("lightsCounter", lightsCounterUniform);
    const auto& matrices = SamuraiCameras::g_activeCamera->GetMVP();
    const glm::mat4 inverseProj = glm::inverse(matrices.projection);
    _lightCullCompute.SetMat4x4("inverseProjection", inverseProj);
    _lightCullCompute.SetMat4x4("view", matrices.view);
    _lightCullCompute.SetMat4x4("projection", matrices.projection);

    // TEMP
    _lightCullCompute.SetIVec2("screenDimension", glm::ivec2(2560, 1440));

    // restoring global index value
    uint32_t newCounter = 0;
    glNamedBufferSubData(_lightIndexList.globalIndexHandle, 0, sizeof(uint32_t), &newCounter);
   
    // ??? to do
    glDispatchCompute(_lightCullInitializers.numGroupsX, _lightCullInitializers.numGroupsY, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

}
