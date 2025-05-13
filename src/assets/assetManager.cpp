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

// Purpose: get center point of the mesh model
// Output: if object was found - vector of position, otherwise std::nullopt
std::optional<glm::vec3> AssetManager::GetMeshCenterPoint(const std::string& entityName)
{
    auto storageIt = _meshCenterCache.find(entityName);
    if(storageIt != _meshCenterCache.end())
        return storageIt->second;
    
    auto nameValidationIt = _assetStorage.find(entityName);
    if(nameValidationIt == _assetStorage.end())
    {
        std::cout << "Mesh by name " << entityName << " is not found in the storage\n";
        return std::nullopt;
    }

    auto indVert = GetMeshStartEndIndices(entityName);
    if(indVert == std::nullopt)
    {
        std::cout << "Mesh start and end indices by name are not found\n";
        return std::nullopt;
    }

    uint32_t startOffset = indVert.value().first;
    const uint32_t endIndex = indVert.value().second;

    const float lowest = std::numeric_limits<float>::lowest();
    const float maximum = std::numeric_limits<float>::max();

    glm::vec3 minPoint = glm::vec3(maximum);
    glm::vec3 maxPoint = glm::vec3(lowest);

    const auto& vertices = _model.get()->GetModelsEBOData().vertices;
    for(; startOffset < endIndex; ++startOffset)
    {
        minPoint.x = std::min(minPoint.x, vertices[startOffset].position.x);
        minPoint.y = std::min(minPoint.y, vertices[startOffset].position.y);
        minPoint.z = std::min(minPoint.z, vertices[startOffset].position.z);

        maxPoint.x = std::max(maxPoint.x, vertices[startOffset].position.x);
        maxPoint.y = std::max(maxPoint.y, vertices[startOffset].position.y);
        maxPoint.z = std::max(maxPoint.z, vertices[startOffset].position.z);
    }

    glm::vec3 centerPoint = (minPoint + maxPoint) / 2.0f;

    _meshCenterCache.insert({entityName, centerPoint});

    return centerPoint;
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


        result.insert(result.end(), allVertices.begin() + it->first, allVertices.begin() + (it->first + it->second));

    }   


    return result;
}

// Purpose: get start and end indices in the buffer to not make undesired copies of
// large buffer when need to extract some mesh vertices
std::optional<std::pair<uint32_t, uint32_t>> AssetManager::GetMeshStartEndIndices(const std::string& entityName) const
{
    const CurrentModelDesc* lookingForModel = GetModelDescriptorByName(entityName);
    if(lookingForModel == nullptr)
    {
        std::cout << "Can't find model by name " << entityName << '\n';
        return std::nullopt;
    }

    const uint32_t maxInt = std::numeric_limits<uint32_t>::max();

    std::pair<uint32_t, uint32_t> result;
    result.first = std::numeric_limits<uint32_t>::max();
    result.second = 0; // as it is unsigned int

    for(auto it = lookingForModel->indOffsetVertCount.begin(); it != lookingForModel->indOffsetVertCount.end(); ++it)
    {
        result.first   = std::min(result.first,  it->first);
        result.second += it->second;
    }

    // end offset: start index + all vertices count for curr mesh
    result.second += result.first;

    if(result.first == std::numeric_limits<uint32_t>::max() && result.second == 0)
    {
        std::cout << "Some error in getting models indices in buffer\n";
        return std::nullopt;
    }

    return result;
}


void AssetManager::AddEntityToLoad(const ObjectDescriptor& objectDesc)
{
    if(!_assetStorage.contains(objectDesc.name))
    {
        _assetStorage.insert({objectDesc.name, {_model->LoadModel(objectDesc)}});
    }
    if(!_assetMatrices.contains(objectDesc.name))
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
	OpenglBackend::BindModelEBO(_model->GetModelsEBOData());
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
