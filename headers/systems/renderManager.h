#pragma once

#include "../types/renderTypes.h"

#include "../assets/assetManager.h"
#include "../types/openglTypes.h"
#include "forwardPlus.h"


namespace RenderManager
{
    extern std::unordered_map<std::string, const CurrentModelDesc*> _meshDispatchesHandle;
    extern std::unordered_map<RenderPassType, std::vector<const CurrentModelDesc*>> _renderTypes;
    extern std::unordered_map<RenderPassType, Shader> _shaderTypes;
    extern std::vector<TextureDesc> _additionalTextures;
    extern std::unordered_set<MatrixDesc, MatrixHashFunc>   _additionalMatrices;
    extern std::unordered_set<VectorDesc, VectorHashFunc>   _additionalVectors;



    void DrawDepthPass(AssetManager& manager, const glm::mat4& viewProj);
    void DrawSkybox(AssetManager& manager);
    void DrawMainScene(AssetManager& manager);
    // textures
    void BindTextures(const ModelTexDesc& textureIds, Shader& shader);
    void UnbindTextures();
    void BindAdditionalTextures(const RenderPassType type);
    void AttachTextureToDraw(const TextureDesc& texDesc);
    // matrices
    void AttachMatrixToBind(const MatrixDesc& matrixDesc);
    void BindAdditionalMatrices(const RenderPassType type, Shader* shader);
    // state change
    void DispatchMeshToDraw(const ObjectDescriptor& objDesc, const AssetManager& manager);
    void AddShaderByType(Shader&& shader, RenderPassType renderType);

     // vectors
     void BindAdditionalVectors(const RenderPassType type, Shader* shader);
     void AttachVectorToBind(const VectorDesc& vectorDesc);
};
