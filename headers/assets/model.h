#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <filesystem>
#include <array>

#include "../types/types.h"
#include "../systems/shaders.h"
#include <map>

struct CurrentModelDesc
{
    ObjectDescriptor objDesc;
    
    std::map<uint32_t, uint32_t> indOffsetVertCount;
    std::vector<ModelTexDesc> textureIDs;
};

class Model
{
private:
    EBOSetup _allModelsData;
    void ProcessNode(aiNode* node, const aiScene* scene,  CurrentModelDesc& modelDescriptor);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene,  CurrentModelDesc& modelDescriptor);
    uint32_t StbiLoadTexture(const char* fileName, bool gamma = false);
    void ProcessMaterial(aiMaterial* material, 
        std::array<aiTextureType, 4> textureTypes, CurrentModelDesc& modelDescriptor);
    
public:
    CurrentModelDesc LoadModel(const ObjectDescriptor& objDescriptor);
    EBOSetup& GetModelsEBOData()  { return _allModelsData;}
};

