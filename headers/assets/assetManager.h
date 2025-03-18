#pragma once
#include "model.h"

#include <memory>
#include <optional>
class AssetManager
{
private:
    std::unique_ptr<Model> _model = std::make_unique<Model>();
    void BindTextures(Shader* shader, const ModelTexDesc& textureIds);
    void UnbindTextures();
    std::unordered_map<std::string, glm::mat4> _assetMatrices;
    std::unordered_map<std::string, CurrentModelDesc> _assetStorage;
public:
    const uint32_t GetAssetsVAO() const { return _model->GetModelsEBOData().VAO;}
    void AddEntityToLoad(ObjectDescriptor objectDesc);
    void BindStructures();
    // void DrawParticularModel(const std::string& entityName);
    void ApplyTransformation(const std::string& entityName, const glm::mat4 modelMat);
    const auto& GetAssetStorage() const { return _assetStorage;}
    const glm::mat4* GetTransformMatrixByName(const std::string& name) const;
    const auto& GetBuffers() const { return _model.get()->GetModelsEBOData();}
    std::optional<std::vector<Vertex>> GetMeshVerticesByName(const std::string& entityName) const;

    // making pointer as it easier to error handle that case
    const CurrentModelDesc* GetModelDescriptorByName(const std::string& entityName) const;
};