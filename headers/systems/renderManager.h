#pragma once

#include "../types/types.h"

#include "../assets/assetManager.h"


enum class EntityType
{
    TYPE_MESH,
    TYPE_SKYBOX
};  


enum class RenderPassType
{
    RENDER_MAIN,
    RENDER_SKYBOX,
    RENDER_DEPTHPASS,
};

namespace RenderManager
{
    extern std::unordered_map<std::string, const CurrentModelDesc*> _meshDispatchesHandle;
    extern std::unordered_map<RenderPassType, std::vector<const CurrentModelDesc*>> _renderTypes;
    extern std::unordered_map<RenderPassType, Shader> _shaderTypes;
    extern std::unordered_map<std::string, uint32_t> _additionalTextures;
    void BindTextures(Shader& shader, const ModelTexDesc& textureIds);
    void UnbindTextures();
    void AttachTextureToDraw(std::string textureShaderName, uint32_t textureId);
    void DispatchMeshToDraw(const std::string& entityName, const AssetManager& manager, EntityType type);
    void AddShaderByType(Shader&& shader, RenderPassType renderType);
    void GlobalDraw(AssetManager& manager);
};  