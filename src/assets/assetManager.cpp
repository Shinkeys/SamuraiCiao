#include "../../headers/assets/assetManager.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/systems/camera.h"



const CurrentModelDesc* AssetManager::GetModelDescriptorByName(const std::string& entityName) const
{
    auto search = _assetStorage.find(entityName);

    if(search == _assetStorage.end())
    {
        std::cout << "Unable to find mesh data by name: " << entityName << "\n";
    }
    else
    {
        return &search->second;
    }
    
    return nullptr;

}

const glm::mat4* AssetManager::GetTransformMatrixByName(const std::string& entityName) const
{
    const auto search = _assetMatrices.find(entityName);

    if(search == _assetMatrices.end())
    {
        std::cout << "Unable to find matrix for name: " << entityName << '\n';
    }
    else
    {
        return &search->second;
    }
    
    
    return nullptr;
}


// Purpose: to find needed vertices from large buffer by mesh name, pack as a vector
// Output: vector containing vertices if found mesh by name, std::nullopt otherwise
std::optional<std::vector<Vertex>> AssetManager::GetMeshVerticesByName(const std::string& entityName) const
{
    std::vector<Vertex>& allVertices = _model.get()->GetModelsEBOData().vertices;

    const CurrentModelDesc* lookingForModel = GetModelDescriptorByName(entityName);
    if(lookingForModel == nullptr)
    {
        std::cout << "Can't find model by name " << entityName << '\n';
        return std::nullopt;
    }

    std::vector<Vertex> result;
    for(auto it = lookingForModel->indOffsetVertCount.begin(); it != lookingForModel->indOffsetVertCount.end(); ++it)
    {
        result.reserve(it->second);

        for(int32_t i = 0; i < it->second; ++i)
        {
            result.push_back(allVertices[i + it->first]);
        }
    }   

    return result;
}


void AssetManager::AddEntityToLoad(ObjectDescriptor objectDesc)
{
    if(_assetStorage.find(objectDesc.name) == _assetStorage.end())
    {
        _assetStorage.insert({objectDesc.name, {_model.get()->LoadModel(objectDesc.name, objectDesc.type)}});
    }
    if(_assetMatrices.find(objectDesc.name) == _assetMatrices.end())
    {
        glm::mat4 model = glm::mat4(1.0f);
        _assetMatrices.insert({objectDesc.name, model});
    }
}


void AssetManager::ApplyTransformation(const std::string& entityName, const glm::mat4 modelMat)
{
    if(_assetMatrices.find(entityName) == _assetMatrices.end())
    {
        std::cout << "Unable to apply transformation to: " << entityName << " it doesn't exist\n";
    }
    _assetMatrices[entityName] = modelMat;
}

void AssetManager::BindStructures()
{
    // binding all models data for opengl
	OpenglBackend::BindModelEBO(_model.get()->GetModelsEBOData());
}


// void AssetManager::DrawParticularModel(const std::string& entityName)
// {
//     auto model = _assetStorage.find(entityName);
//     if(model == nullptr)
//     {
//         std::cout << "Cannot draw. Model is not found\n";
//         return;
//     }

//     // count of parts of model
//     const uint32_t partsOfModel = model->second.second.currMeshVertCount.size();
//     for(uint32_t i = 0; i < partsOfModel; ++i)
//     {
//         const uint32_t vertexCount = model->second.second.currMeshVertCount[i];
//         const uint32_t offset = model->second.second.meshIndexOffset[i];
//         glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 
//             (void*)(offset + _model.get()->GetModelsEBOData().indices.data()));
//     }

// }
