#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/camera.h"

namespace RenderManager
{
    std::unordered_map<std::string, const CurrentModelDesc*> _meshDispatchesHandle;
    std::unordered_map<RenderPassType, std::vector<const CurrentModelDesc*>> _renderTypes;
    std::unordered_map<RenderPassType, Shader> _shaderTypes;
}

void RenderManager::DispatchMeshToDraw(const std::string& entityName, const AssetManager& manager, EntityType type)
{
    const CurrentModelDesc* modelDescriptor = manager.GetModelDescriptorByName(entityName);

    if(modelDescriptor == nullptr)
    {
        std::cout << "Model descriptor is nullptr, that means model is not found by name: " << entityName << '\n';
        return;
    }

    std::string meshName = entityName;
    _meshDispatchesHandle.insert({meshName, modelDescriptor});

    switch(type)
    {
    case EntityType::TYPE_MESH:
        _renderTypes[RenderPassType::RENDER_MAIN].push_back(modelDescriptor);
        break;
    case EntityType::TYPE_SKYBOX:
        _renderTypes[RenderPassType::RENDER_SKYBOX].push_back(modelDescriptor);
        break;
    default: std::cout << "No type of model dispatch\n";
        break;
    }
}

// void RenderManager::SortDataByShaders()
// {
    
// }

void RenderManager::AddShaderByType(Shader&& shader, RenderPassType renderType)
{
    switch(renderType)
    {
    case RenderPassType::RENDER_MAIN:
        _shaderTypes.emplace(renderType, std::move(shader));
        break;
    case RenderPassType::RENDER_SKYBOX:
        _shaderTypes.emplace(renderType, std::move(shader));
        break;

    default: std::cout << "No type of passed shader: " << shader.GetShaderName() << '\n';
        break;
    }
    
}

void RenderManager::GlobalDraw(AssetManager& manager)
{

    if(_renderTypes.size() == 0)
    {
        std::cout << "No commands to draw something\n";
        return;
    }

    const glm::mat4 standardModelMatrix = glm::mat4(1.0f);

    auto shaderSkybox = _shaderTypes.find(RenderPassType::RENDER_SKYBOX);
    if(shaderSkybox == _shaderTypes.end())
        std::cout << "Shader for skybox render pass is not found\n";
    else 
    {
        shaderSkybox->second.UseShader();
        // setting matrices
        shaderSkybox->second.SetMat4x4("view", Camera::GetMVP().view);
        shaderSkybox->second.SetMat4x4("projection", Camera::GetMVP().projection);
        for(const auto skybox : _renderTypes.find(RenderPassType::RENDER_SKYBOX)->second)
        {
            for(uint32_t i = 0; i < skybox->currMeshVertCount.size(); ++i)
            {
                BindTextures(shaderSkybox->second, skybox->textureIDs[i]);
                const uint32_t vertexCount = skybox->currMeshVertCount[i];
                const uint32_t offset = skybox->meshIndexOffset[i];
                glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                    (void*)(offset + manager.GetBuffers().indices.data()));
                UnbindTextures();
            }
        }
    }

    auto shaderMain = _shaderTypes.find(RenderPassType::RENDER_MAIN);
    if(shaderMain == _shaderTypes.end())
        std::cout << "Shader for main render pass is not found\n";
    else shaderMain->second.UseShader();
    for(const auto mesh : _renderTypes.find(RenderPassType::RENDER_MAIN)->second)
    {
        // finding bounded transformations to current entity
        const glm::mat4* transformation = manager.GetTransformMatrixByName(mesh->modelName);
        if(transformation == nullptr)
        {
            shaderMain->second.SetMat4x4("model", standardModelMatrix);
        }
        else if(shaderMain != nullptr)
        {
            shaderMain->second.SetMat4x4("view", Camera::GetMVP().view);
            shaderMain->second.SetMat4x4("projection", Camera::GetMVP().projection);

            shaderMain->second.SetMat4x4("model", *transformation);
            const glm::mat4& currModelMatrix = *transformation;
            const glm::mat4& currViewMatrix = Camera::GetMVP().view;
            const glm::mat4 normalMatrix = 
            glm::transpose(glm::inverse(currViewMatrix * currModelMatrix));
            shaderMain->second.SetMat4x4("normalMatrix", normalMatrix);
        }
    
        for(uint32_t i = 0; i < mesh->currMeshVertCount.size(); ++i)
        {
            // binding textures
            if(shaderMain != nullptr)
                BindTextures(shaderMain->second, mesh->textureIDs[i]);

            const uint32_t vertexCount = mesh->currMeshVertCount[i];
            const uint32_t offset = mesh->meshIndexOffset[i];
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                (void*)(offset + manager.GetBuffers().indices.data()));
            
            if(shaderMain != nullptr)
                UnbindTextures();
        }
    }

}


void RenderManager::BindTextures(Shader& shader, const ModelTexDesc& textureIds)
{
    const int32_t diffusePlace = 1;
    const int32_t specularPlace = 2;
    const int32_t normalPlace = 3;
    const int32_t emissionPlace = 4;

    shader.SetUniform1i("textures.diffuse", 1);
    if(textureIds.diffuseId > 0)
    {
        glBindTextureUnit(1, textureIds.diffuseId);
    }
    shader.SetUniform1i("textures.specular", 2);
    if(textureIds.specularId > 0)
    {
        glBindTextureUnit(2, textureIds.specularId);
    }
    shader.SetUniform1i("textures.normal", 3);
    if(textureIds.normalId > 0)
    {
        glBindTextureUnit(3, textureIds.normalId);
    }
    shader.SetUniform1i("textures.emission", 4);
    if(textureIds.emissionId > 0)
    {
        glBindTextureUnit(4, textureIds.emissionId);
    }
}

void RenderManager::UnbindTextures()
{
    // getting count of elements to not write manually
    const int32_t textureCount = sizeof(ModelTexDesc) / sizeof(ModelTexDesc::diffuseId);

    for(int32_t i = 0; i < textureCount; ++i)
    {
        glBindTextureUnit(i + 1, 0);
    }
}