#include "../../headers/systems/renderManager.h"
#include "../../headers/systems/camera.h"

namespace RenderManager
{
    std::unordered_map<std::string, const CurrentModelDesc*> _meshDispatchesHandle;
    std::unordered_map<RenderPassType, std::vector<const CurrentModelDesc*>> _renderTypes;
    std::unordered_map<RenderPassType, Shader> _shaderTypes;

    std::unordered_set<TextureDesc, TextureHashFunc> _additionalTextures;
    std::unordered_set<MatrixDesc, MatrixHashFunc> _additionalMatrices;
    template <typename VecType>
    std::unordered_set<VectorDesc<VecType>, VectorHashFunc<VecType>> _additionalVectors;
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
        // RenderManager::AttachTextureToDraw();

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
    case RenderPassType::RENDER_DEPTHPASS:
        _shaderTypes.emplace(renderType, std::move(shader));
        break;
    default: std::cout << "No type of passed shader: " << shader.GetShaderName() << '\n';
        break;
    }
    
}

void RenderManager::AttachTextureToDraw(const TextureDesc& texDesc)
{
    _additionalTextures.insert(texDesc);
}

void RenderManager::AttachMatrixToBind(const MatrixDesc& matrixDesc)
{
    _additionalMatrices.insert(matrixDesc);
}

template <typename VecType>
void RenderManager::AttachVectorToBind(const VectorDesc<VecType>& vectorDesc)
{
    _additionalVectors<VecType>.insert(vectorDesc);
}


void RenderManager::GlobalDraw(AssetManager& manager)
{

    if(_renderTypes.size() == 0)
    {
        std::cout << "No commands to draw something\n";
        return;
    }

    DrawSkybox(manager);
    DrawMainScene(manager);



}

template <typename VecType>
void RenderManager::BindAdditionalVectors(const RenderPassType type, Shader* shader)
{
    if(_additionalMatrices.size() == 0)
    {
        // std::cout << "No commands to bind additional vectors\n";
        return;
    }

    if(shader == nullptr)
    {
        std::cout << "Cant bind additional matrix. Shader is nullptr\n";
    }

    for(const auto& vector : _additionalVectors<VecType>)
    {
        if(vector.type == type)
        {
            switch(vector.dimension)
            {
            case VectorDimension::DIM_VEC2:
                shader->SetVec2(vector.name, vector.data);
                break;
            case VectorDimension::DIM_VEC3:
                shader->SetVec3(vector.name, vector.data);
                break;
            case VectorDimension::DIM_VEC4:
                shader->SetVec4(vector.name, vector.data);
                break;
            default: 
                std::cout << "No such type of vector to bind to render\n";
                break;
            }
        }
    }
}

void RenderManager::BindAdditionalMatrices(const RenderPassType type, Shader* shader)
{
    if(_additionalMatrices.size() == 0)
    {
        // std::cout << "No commands to bind additional matrices\n";
        return;
    }

    if(shader == nullptr)
    {
        std::cout << "Cant bind additional matrix. Shader is nullptr\n";
    }

    for(const auto& matrix : _additionalMatrices)
    {
        if(matrix.type == type)
        {
            shader->SetMat4x4(matrix.name, matrix.data);
        }
    }
}


void RenderManager::BindAdditionalTextures(const RenderPassType type, Shader* shader)
{
    if(_additionalTextures.size() == 0)
    {
        // std::cout << "No commands to bind additional textures\n";
        return;
    }

    if(shader == nullptr)
    {
        std::cout << "Cant bind additional texture. Shader is nullptr\n";
        return;
    }

    const int32_t mainShaderTextureCount = sizeof(ModelTexDesc) / sizeof(ModelTexDesc::diffuseId);

    int32_t currentTextureId = 1;
    for(const auto& texture : _additionalTextures)
    {
        if(type == RenderPassType::RENDER_MAIN)
        {
            currentTextureId = mainShaderTextureCount + 1;    
        }

        shader->SetUniform1i(texture.name, currentTextureId); 
        glBindTextureUnit(currentTextureId, texture.handle);
        ++currentTextureId;
    }
    
}

void RenderManager::DrawMainScene(AssetManager& manager)
{
    const RenderPassType passType = RenderPassType::RENDER_MAIN;


    const glm::mat4 standardModelMatrix = glm::mat4(1.0f);
    auto shaderMainIt = _shaderTypes.find(passType);
    if(shaderMainIt == _shaderTypes.end())
    {
        std::cout << "Shader for main render pass is not found\n";
        return;
    }
    
    shaderMainIt->second.UseShader();

    for(const auto mesh : _renderTypes.find(passType)->second)
    {
        // finding bounded transformations to current entity
        const glm::mat4* transformation = manager.GetTransformMatrixByName(mesh->modelName);
        if(transformation == nullptr)
        {
            shaderMainIt->second.SetMat4x4("model", standardModelMatrix);
        }
        else if(shaderMainIt != _shaderTypes.end())
        {
            shaderMainIt->second.SetMat4x4("view", Camera::GetMVP().view);
            shaderMainIt->second.SetMat4x4("projection", Camera::GetMVP().projection);

            shaderMainIt->second.SetMat4x4("model", *transformation);
            const glm::mat4& currModelMatrix = *transformation;
            const glm::mat4& currViewMatrix = Camera::GetMVP().view;
            const glm::mat4 normalMatrix = 
            glm::transpose(glm::inverse(currViewMatrix * currModelMatrix));
            shaderMainIt->second.SetMat4x4("normalMatrix", normalMatrix);

        }
    
        for(uint32_t i = 0; i < mesh->currMeshVertCount.size(); ++i)
        {
            // binding textures
            if(shaderMainIt != _shaderTypes.end())
                BindTextures(mesh->textureIDs[i]);
            
            const uint32_t vertexCount = mesh->currMeshVertCount[i];
            const uint32_t offset = mesh->meshIndexOffset[i];
            glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                (void*)(offset + manager.GetBuffers().indices.data()));
            
            if(shaderMainIt != _shaderTypes.end())
                UnbindTextures();
        }
    }

    // binding additional textures
    BindAdditionalTextures(passType, &shaderMainIt->second);
    BindAdditionalMatrices(passType, &shaderMainIt->second);
}


void RenderManager::DrawSkybox(AssetManager& manager)
{
    const RenderPassType passType = RenderPassType::RENDER_SKYBOX;
    auto shaderSkyboxIt = _shaderTypes.find(passType);
    if(shaderSkyboxIt == _shaderTypes.end())
        std::cout << "Shader for skybox render pass is not found\n";
    else 
    {
        shaderSkyboxIt->second.UseShader();
        // setting matrices
        shaderSkyboxIt->second.SetMat4x4("view", Camera::GetMVP().view);
        shaderSkyboxIt->second.SetMat4x4("projection", Camera::GetMVP().projection);
        for(const auto skybox : _renderTypes.find(passType)->second)
        {
            for(uint32_t i = 0; i < skybox->currMeshVertCount.size(); ++i)
            {
                BindTextures(skybox->textureIDs[i]);
                const uint32_t vertexCount = skybox->currMeshVertCount[i];
                const uint32_t offset = skybox->meshIndexOffset[i];
                glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
                    (void*)(offset + manager.GetBuffers().indices.data()));
                UnbindTextures();
            }
        }
    }
    // binding additional textures
    BindAdditionalTextures(passType, &shaderSkyboxIt->second);
    BindAdditionalMatrices(passType, &shaderSkyboxIt->second);
}

void RenderManager::BindTextures(const ModelTexDesc& textureIds)
{
    const int32_t diffusePlace = 1;
    const int32_t specularPlace = 2;
    const int32_t normalPlace = 3;
    const int32_t emissionPlace = 4;

    if(textureIds.diffuseId > 0)
    {
        glBindTextureUnit(diffusePlace, textureIds.diffuseId);
    }
    if(textureIds.specularId > 0)
    {
        glBindTextureUnit(specularPlace, textureIds.specularId);
    }
    if(textureIds.normalId > 0)
    {
        glBindTextureUnit(normalPlace, textureIds.normalId);
    }
    if(textureIds.emissionId > 0)
    {
        glBindTextureUnit(emissionPlace, textureIds.emissionId);
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