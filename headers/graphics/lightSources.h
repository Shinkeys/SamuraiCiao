#pragma once
#include "../types/lightTypes.h"
#include "../systems/shaders.h"
#include "../assets/assetManager.h"

// Would use char to make better compatibility with imgui
struct CreateLightDesc
{
    char nameBuffer[33];
    float position[3];
    float color[3];
    LightType type;
    bool affectsShadows;
};

// Purpose: can't send plain data from unordered_map or array to the opengl
// so need to use some data handle
struct LightBuffersHandle
{
    uint32_t lightsBindID;
    uint32_t lightsHandle;
    std::vector<LightDescriptor> lights;
};

enum class LightUpdateCommand
{
    ADD_LIGHT,
    REMOVE_LIGHT,
    UPDATE_LIGHT,
};

class LightSources
{
private:
    std::unordered_map<std::string, LightDescriptor> _lightsStorage;
    glm::vec3 _directionalLightDir = glm::vec3(0.0f, 1.5f, 4.5f);


    std::array<std::pair<std::string, LightsAffectingShadowsDesc>, LightDefines::g_max_lights_affecting_shadows> _lightsForShadowsStorage;
    CreateLightDesc _createLightDesc { };

    LightBuffersHandle _lightsBufferHandle { };

    void AddLightSourceDesc(LightDescriptor& lightDesc, const std::string& lightName, bool affectShadows);
    // If this light source affects shadows in the scene
    void AddLightSourceWithShadowInfluence(const std::string& lightName, LightDescriptor& lightDesc);

    void CreateLightBuffers();

    void UpdateLightBuffer(LightUpdateCommand command, const std::string& lightName);
public:
    void InterfaceLightsCreation();
    const auto& GetLightSources() const { return _lightsStorage;}
    const auto& GetLightsInfluencingShadows() const { return _lightsForShadowsStorage; }
    const LightBuffersHandle& GetLightBuffersHandle() const { return _lightsBufferHandle; }
    glm::vec3 GetDirectionalLightDir() const { return _directionalLightDir; }
    void Prepare();
};