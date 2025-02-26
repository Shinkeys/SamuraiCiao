#pragma once

#include "../types/renderTypes.h"

#include "../assets/assetManager.h"



namespace RenderManager
{
    extern std::unordered_map<std::string, const CurrentModelDesc*> _meshDispatchesHandle;
    extern std::unordered_map<RenderPassType, std::vector<const CurrentModelDesc*>> _renderTypes;
    extern std::unordered_map<RenderPassType, Shader> _shaderTypes;
    extern std::vector<TextureDesc> _additionalTextures;
    void BindTextures(Shader& shader, const ModelTexDesc& textureIds);
    void DrawSkybox(AssetManager& manager);
    void DrawMainScene(AssetManager& manager);
    void UnbindTextures();
    void BindAdditionalTextures(const RenderPassType type, Shader* shader);
    void AttachTextureToDraw(const TextureDesc& texDesc);
    void DispatchMeshToDraw(const std::string& entityName, const AssetManager& manager, EntityType type);
    void AddShaderByType(Shader&& shader, RenderPassType renderType);
    void GlobalDraw(AssetManager& manager);
};  