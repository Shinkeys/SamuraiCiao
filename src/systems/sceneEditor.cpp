#include "../../headers/systems/sceneEditor.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/renderManager.h"

void SceneEditor::PrepareObjectSelection(uint32_t width, uint32_t height)
{
    if(width <= 0 || height <= 0)
    {
        std::cout << "Width or height is 0\n";
        return;
    }

    _viewportExtent.first = width;
    _viewportExtent.second  = height;

    // texture which store meshes data to pick
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_colorTex);
    glBindTexture(GL_TEXTURE_2D, _colorTex);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, width, 
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

void SceneEditor::DrawScene(const AssetManager& manager)
{
    if(_viewportExtent.first <= 0 || _viewportExtent.second <= 0)
    {
        std::cout << "Can't draw scene for scene editor, viewport extent is 0 or less\n";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    OpenglBackend::SetViewport(_viewportExtent.first, _viewportExtent.second);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_SCENE_EDITOR);
    if(shaderIt == RenderManager::_shaderTypes.end())
    {
        std::cout << "Can't draw scene for scene editor, shader is not found\n";
        return;
    }

    shaderIt->second.UseShader();
    for(const auto& mesh : manager.GetAssetStorage())
    {
        const glm::mat4* modelMat = manager.GetTransformMatrixByName(mesh.second.modelName);
        if(modelMat != nullptr)
            shaderIt->second.SetMat4x4("model", *modelMat);
        else std::cout << "Model matrix for shadows not found\n";


        for(auto it = mesh.second.indOffsetVertCount.begin(); it != mesh.second.indOffsetVertCount.end(); ++it)
        {
            const uint32_t vertexCount = it->second;
            const uint32_t offset = it->first;
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                (void*)(offset + manager.GetBuffers().indices.data()));
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneEditor::HandleObjectSelection(const Window& window, const Matrices& matrices, const AssetManager& manager)
{
    //  drawing scene with id to pick later with mouse
    DrawScene(manager);

    // handle picking by itself
    SelectObject(window, matrices);
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