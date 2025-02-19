#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include <filesystem>

#include <string>
#include <unordered_map>

#include "../types/types.h"

enum class OperationType
{
    SHADER_CREATE,
    SHADER_LINK,
};

class Shader
{
private:
    std::string ReadFromFile(std::filesystem::path path);
    int32_t CheckForErrors(uint32_t shaderID, OperationType type);
    std::unordered_map<std::string, uint32_t> _uniformLocations;
    GLint _programID = -1;
    
public:
    void UseShader();
    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3x3(const std::string& name, const glm::mat3& value);
    void SetMat4x4(const std::string& name, const glm::mat4& value);
    void SetUniform1i(const std::string& name, int value);
    void LoadShaders(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

};
