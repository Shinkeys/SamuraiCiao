#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <filesystem>
#include <array>

#include "../types/types.h"
#include "../systems/shaders.h"


struct CurrentModelDesc
{
	std::vector<uint32_t> meshIndexOffset;
	std::vector<uint32_t> currMeshVertCount;
    std::vector<ModelTexDesc> textureIDs;
};

class Model
{
private:
    EBOSetup _allModelsData;
    void ProcessNode(aiNode* node, const aiScene* scene,  CurrentModelDesc& modelDescriptor);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene,  CurrentModelDesc& modelDescriptor);
    std::vector<std::filesystem::path> ProvideModelPaths();
    uint32_t StbiLoadTexture(const char* fileName, bool gamma = false);
    void ProcessMaterial(aiMaterial* material, 
        std::array<aiTextureType, 4> textureTypes, CurrentModelDesc& modelDescriptor);
    
public:
    CurrentModelDesc LoadModel(const std::filesystem::path& modelName);
    EBOSetup& GetModelsEBOData()  { return _allModelsData;}
};

