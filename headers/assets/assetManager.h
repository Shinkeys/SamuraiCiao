#pragma once
#include "model.h"

#include <memory>
class AssetManager
{
private:
    std::unique_ptr<Model> _model = std::make_unique<Model>();
    void BindTextures(Shader* shader, const ModelTexDesc& textureIds);
    void UnbindTextures();
    std::unordered_map<std::string, glm::mat4> _assetMatrices;
    std::unordered_map<std::string, std::pair<Shader, CurrentModelDesc>> _assetStorage;
    std::unordered_map<std::string, glm::vec3> _lightSourcesPositions;
public:
    void AddEntityToLoad(const std::string& entityName, Shader& shader);
    void BindStructures();
    void GlobalDraw();
    void AddLightSourcePos(const std::string& entityName, glm::vec3 pos);
    void DrawParticularModel(const std::string& entityName);
    void ApplyTransformation(const std::string& entityName, const glm::mat4& modelMat);
    const auto& GetAssetStorage() const { return _assetStorage;}
    const auto& GetTransformMatrix() const { return _assetMatrices;}
    const auto& GetLightSources() const { return _lightSourcesPositions;}
};