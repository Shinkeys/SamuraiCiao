#include "../../headers/assets/assetManager.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"

void AssetManager::AddEntityToLoad(std::string entityName, Shader& shader)
{
    if(_assetStorage.find(entityName) == _assetStorage.end())
    {
        _assetStorage.insert({entityName, {shader, _model.get()->LoadModel(entityName)}});
    }
    if(_assetMatrices.find(entityName) == _assetMatrices.end())
    {
        Matrices matrices;
        _assetMatrices.insert({entityName, matrices});
    }
}

void AssetManager::ApplyMVPMatrices(std::string entityName, const Matrices& matrices)
{
    _assetMatrices[entityName] = matrices;
}

void AssetManager::BindStructures()
{
    // binding all models data for opengl
	OpenglBackend::BindModelEBO(_model.get()->GetModelsEBOData());
}


void AssetManager::GlobalDraw()
{
    if(_assetStorage.size() == 0)
    {
        std::cout << "Unable to draw! Container is empty\n";
        return;
    }
    Shader* lastShader = nullptr;

    for(auto it = _assetStorage.begin(); it != _assetStorage.end(); ++it)
    {
        // checking for last binded shader, if it's already binded then skip, otherwise would bind shader
        if(&it->second.first != lastShader)
        {
            it->second.first.UseShader();
            it->second.first.SetMat4x4("view", Camera::GetMVP().view);
            it->second.first.SetMat4x4("projection", Camera::GetMVP().projection);
        }
        lastShader = &it->second.first;
        if(lastShader->GetShaderName() == "main.vert")
        {    
            const glm::mat4 currModelMatrix = _assetMatrices[it->first].model;
            const glm::mat4 currViewMatrix = Camera::GetMVP().view;
            const glm::mat4 normalMatrix = 
            glm::transpose(glm::inverse(currViewMatrix * currModelMatrix));
            lastShader->SetMat4x4("normalMatrix", normalMatrix);
        }
        
        for(uint32_t i = 0; i < it->second.second.currMeshVertCount.size(); ++i)
        {
            lastShader->SetMat4x4("model", _assetMatrices[it->first].model);
            BindTextures(lastShader, it->second.second.textureIDs[i]);
            const uint32_t vertexCount = it->second.second.currMeshVertCount[i];
            const uint32_t offset = it->second.second.meshIndexOffset[i];
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                (void*)(offset + _model.get()->GetModelsEBOData().indices.data()));
            
            // drew mesh, removing textures as sampler would contain it until would get next
            UnbindTextures();
        }
        
    }
}

void AssetManager::BindTextures(Shader* shader, const ModelTexDesc& textureIds)
{
    const int32_t diffusePlace = 1;
    const int32_t specularPlace = 2;
    const int32_t normalPlace = 3;
    const int32_t emissionPlace = 4;

    shader->SetUniform1i("textures.diffuse", 1);
    if(textureIds.diffuseId > 0)
    {
        glBindTextureUnit(1, textureIds.diffuseId);
    }
    shader->SetUniform1i("textures.specular", 2);
    if(textureIds.specularId > 0)
    {
        glBindTextureUnit(2, textureIds.specularId);
    }
    shader->SetUniform1i("textures.normal", 3);
    if(textureIds.normalId > 0)
    {
        glBindTextureUnit(3, textureIds.normalId);
    }
    shader->SetUniform1i("textures.emission", 4);
    if(textureIds.emissionId > 0)
    {
        glBindTextureUnit(4, textureIds.emissionId);
    }
}

void AssetManager::UnbindTextures()
{
    // getting count of elements to not write manually
    const int32_t textureCount = sizeof(ModelTexDesc) / sizeof(ModelTexDesc::diffuseId);

    for(int32_t i = 0; i < textureCount; ++i)
    {
        glBindTextureUnit(i + 1, 0);
    }
}