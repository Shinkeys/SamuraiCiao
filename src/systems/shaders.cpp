#include "../../headers/systems/shaders.h"
#include <fstream>
#include <sstream>


std::string Shader::ReadFromFile(std::filesystem::path path)
{
    std::ifstream readFile(path);
    if(!readFile.is_open())
    {
        std::cout << "Unable to open a file by provided path!\n";
    }
                
    std::stringstream buffer;

    // reading files data to a buffers
    buffer << readFile.rdbuf();

    readFile.close();

    const std::string dataStr = buffer.str();

    return dataStr;
}

int32_t Shader::CheckForErrors(uint32_t operationID, OperationType type)
{
    GLint successCode = 0;
    char log[1024];
    if(type == OperationType::SHADER_CREATE)
    {
        glGetShaderiv(operationID, GL_COMPILE_STATUS, &successCode);
        if(!successCode)
        {
            glGetShaderInfoLog(operationID, 1024, nullptr, log);
            std::cout << "Shader compilation error\nLog: "  << log << std::endl;
            // error in compilation
            return -1;
        }
    }
    else if(type == OperationType::SHADER_LINK)
    {
        glGetProgramiv(operationID, GL_LINK_STATUS, &successCode);
        if(!successCode)
        {
            glGetProgramInfoLog(operationID, 1024, nullptr, log);
            std::cout << "Shader linking error\nLog: "  << log << std::endl;
            return -1;
        }
    }
    return 1;
}

void Shader::LoadShaders(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
{
    std::filesystem::path vertexModifiedPath = "shaders/" / vertexPath;
    std::filesystem::path fragmentModifiedPath = "shaders/" / fragmentPath;

    // need this strings, otherwise c_str would be dangling pointer
    std::string vertexDataStr = ReadFromFile(vertexModifiedPath);
    std::string fragmentDataStr = ReadFromFile(fragmentModifiedPath);

    const char* vertexData = vertexDataStr.c_str();
    const char* fragmentData = fragmentDataStr.c_str();
        
    int32_t errorsCode = 1;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // setting glsl source code to shader
    glShaderSource(vertexShader, 1, &vertexData, nullptr);
    glCompileShader(vertexShader);
    errorsCode = CheckForErrors(vertexShader, OperationType::SHADER_CREATE);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // setting glsl source code to shader
    glShaderSource(fragmentShader, 1, &fragmentData, nullptr);
    glCompileShader(fragmentShader);
    errorsCode = CheckForErrors(fragmentShader, OperationType::SHADER_CREATE);

    _programID = glCreateProgram();
    glAttachShader(_programID, vertexShader);
    glAttachShader(_programID, fragmentShader);
    glLinkProgram(_programID);
    errorsCode = CheckForErrors(_programID, OperationType::SHADER_LINK);

    if(errorsCode == -1)
    {
        if(_programID != -1)
        {
            glDeleteProgram(_programID);
            _programID = -1;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
}


void Shader::UseShader()
{
    glUseProgram(_programID);
}

void Shader::SetBool(const std::string& name, bool value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniform1i(_uniformLocations[name], static_cast<int>(value));
}
void Shader::SetInt(const std::string& name, int value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniform1i(_uniformLocations[name], value);
}
void Shader::SetFloat(const std::string& name, float value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniform1f(_uniformLocations[name], value);
}
void Shader::SetVec4(const std::string& name, const glm::vec4& value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniform4fv(_uniformLocations[name], 1, &value[0]);
}
void Shader::SetVec3(const std::string& name, const glm::vec3& value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniform3fv(_uniformLocations[name], 1, &value[0]);
}
void Shader::SetVec2(const std::string& name, const glm::vec2& value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniform2fv(_uniformLocations[name], 1, &value[0]);
}
void Shader::SetMat3x3(const std::string& name, const glm::mat3& value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniformMatrix3fv(_uniformLocations[name], 1, GL_FALSE, &value[0][0]);
}
void Shader::SetMat4x4(const std::string& name, const glm::mat4& value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniformMatrix4fv(_uniformLocations[name], 1, GL_FALSE, &value[0][0]);
}
void Shader::SetUniform1i(const std::string& name, int32_t value)
{
    if(_uniformLocations.find(name) == _uniformLocations.end())
        _uniformLocations[name] = glGetUniformLocation(_programID, name.c_str());
    glUniform1i(_uniformLocations[name], value);
}


