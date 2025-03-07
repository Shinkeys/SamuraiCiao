#include "../../headers/systems/sceneEditor.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/renderManager.h"

void SceneEditor::Prepare(uint32_t width, uint32_t height)
{
    if(width <= 0 || height <= 0)
    {
        std::cout << "Width or height is 0\n";
        return;
    }
    // texture which store meshes data to pick
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_colorTex);
    glBindTexture(GL_TEXTURE_2D, _colorTex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8UI, width, 
        height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, nullptr);

    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        _colorTex, 0);

    // texture with depth to check for the first obj on ray
    glGenTextures(1, &_depthTex);
    glBindTexture(GL_TEXTURE_2D, _depthTex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, 
        height, 0, GL_DEPTH_COMPONENT16, GL_FLOAT, nullptr);
    
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
    _depthTex, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Framebuffer incomplete\n";
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);   

    Shader sceneEditorShader;
    sceneEditorShader.LoadShaders("sceneEditor.vert", "sceneEditor.frag");
    
    RenderManager::AddShaderByType(std::move(sceneEditorShader), RenderPassType::RENDER_SCENE_EDITOR);
}

void SceneEditor::DrawScene(AssetManager& manager)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    OpenglBackend::SetViewport(_viewportExtent.first, _viewportExtent.second);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_SCENE_EDITOR);
    if(shaderIt == RenderManager::_shaderTypes.end())
    {
        std::cout << "Can't draw scene for scene editor, shader is not found\n";
        return;
    }
    for(const auto& mesh : manager.GetAssetStorage())
    {
        const glm::mat4* modelMat = manager.GetTransformMatrixByName(mesh.second.modelName);
        if(modelMat != nullptr)
            shaderIt->second.SetMat4x4("model", *modelMat);
        else std::cout << "Model matrix for shadows not found\n";

        for(uint32_t i = 0; i < mesh.second.currMeshVertCount.size(); ++i)
        {
            const uint32_t vertexCount = mesh.second.currMeshVertCount[i];
            const uint32_t offset = mesh.second.meshIndexOffset[i];
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                (void*)(offset + manager.GetBuffers().indices.data()));
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SceneEditor::SelectObject(const Window& window, const Matrices& matrices)
{
    const Mouse& mouse = window.GetMousePositions();
    const uint32_t width = window.GetWindowWidth();
    const uint32_t height = window.GetWindowHeight();
    if(window.GetMousePositions().clicked)
    {
        // making normalize device coords from mouse pos
        const float x = (2.0f * mouse.x) / static_cast<float>(width) - 1.0f;
        const float y = 1.0f - (2.0f * mouse.y) / static_cast<float>(height);
        const float z = 1.0f;

        // making z point forwards in homo clip space
        glm::vec4 rayClip = glm::vec4(x, y, -z, 1.0);
        // going to view space
        glm::vec4 rayViewSpace = glm::inverse(matrices.projection) * rayClip;
        rayViewSpace = glm::vec4(rayViewSpace.x, rayViewSpace.y, -z, 0.0);

        // maybe to do in world space, would see
        _rayCoords = glm::normalize(rayViewSpace);


    }
}