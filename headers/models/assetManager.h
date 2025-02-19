#pragma once
#include "model.h"

#include <memory>
class AssetManager
{
private:
    std::unique_ptr<Model> _model = std::make_unique<Model>();
    void BindTextures(Shader* shader, const ModelTexDesc& textureIds);
    void UnbindTextures();
    std::unordered_map<std::string, Matrices> _assetMatrices;
    std::unordered_map<std::string, std::pair<Shader*, CurrentModelDesc>> _assetStorage;
public:
    void AddEntityToLoad(std::string entityName, Shader& shader);
    void BindStructures();
    void GlobalDraw();
    void ApplyMVPMatrices(std::string entityName, const Matrices& matrices);
};