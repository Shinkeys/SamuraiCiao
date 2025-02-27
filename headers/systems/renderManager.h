#pragma once

#include "../types/renderTypes.h"

#include "../assets/assetManager.h"



namespace RenderManager
{
    extern std::unordered_map<std::string, const CurrentModelDesc*> _meshDispatchesHandle;
    extern std::unordered_map<RenderPassType, std::vector<const CurrentModelDesc*>> _renderTypes;
    extern std::unordered_map<RenderPassType, Shader> _shaderTypes;
    extern std::unordered_set<TextureDesc, TextureHashFunc> _additionalTextures;
    extern std::unordered_set<MatrixDesc, MatrixHashFunc> _additionalMatrices;
    template <typename VecType>
    extern std::unordered_set<VectorDesc<VecType>, VectorHashFunc<VecType>> _additionalVectors;


    void DrawSkybox(AssetManager& manager);
    void DrawMainScene(AssetManager& manager);
    // textures
    void BindTextures(const ModelTexDesc& textureIds);
    void UnbindTextures();
    void BindAdditionalTextures(const RenderPassType type, Shader* shader);
    void AttachTextureToDraw(const TextureDesc& texDesc);
    // matrices
    void AttachMatrixToBind(const MatrixDesc& matrixDesc);
    void BindAdditionalMatrices(const RenderPassType type, Shader* shader);
    void DispatchMeshToDraw(const std::string& entityName, const AssetManager& manager, EntityType type);
    void AddShaderByType(Shader&& shader, RenderPassType renderType);
    void GlobalDraw(AssetManager& manager);
    // vectors
    template <typename VecType>
    void BindAdditionalVectors(const RenderPassType type, Shader* shader);
    template <typename VecType>
    void AttachVectorToBind(const VectorDesc<VecType>& vectorDesc);
};  