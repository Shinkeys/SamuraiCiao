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

const glm::mat4* AssetManager::GetTransformMatrixByName(const std::string& entityName)
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


void AssetManager::AddEntityToLoad(const std::string entityName)
{
    if(_assetStorage.find(entityName) == _assetStorage.end())
    {
        _assetStorage.insert({entityName, {_model.get()->LoadModel(entityName)}});
    }
    if(_assetMatrices.find(entityName) == _assetMatrices.end())
    {
        glm::mat4 model = glm::mat4(1.0f);
        _assetMatrices.insert({entityName, model});
    }
}

void AssetManager::AddLightSourcePos(const std::string entityName, glm::vec3 pos)
{
    if(_lightSourcesPositions.find(entityName) == _lightSourcesPositions.end())
    {
        _lightSourcesPositions.insert({entityName, pos});
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
