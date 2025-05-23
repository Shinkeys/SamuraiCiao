#pragma once
#include <glad/glad.h>
#include <iostream>
#include "../types/types.h"


enum class ErrorCodes_Backend
{
    NO_ERROR,
    ERROR_SSBO_CREATION,
    ERROR_VBO_BINDING,
    ERROR_EBO_BINDING,
};

namespace OpenglBackend
{
    inline void CreateVAO(uint32_t& vao) { glGenVertexArrays(1, &vao);}
    inline void CreateVBO(uint32_t& vbo) { glGenBuffers(1, &vbo);}
    inline void CreateEBO(uint32_t& ebo) { glGenBuffers(1, &ebo);}
    ErrorCodes_Backend BindModelVBO(VBOSetupUnskinned& setup);
    ErrorCodes_Backend BindModelVBO(VBOSetupUnskinnedVec4& setup);
    ErrorCodes_Backend BindModelEBO(EBOSetup& setup);
    ErrorCodes_Backend BindModelEBO(EBOSetupUnskinned& setup);
    ErrorCodes_Backend BindModelEBO(EBOSetupBuffers& buffers, std::vector<uint32_t>& indices, std::vector<glm::vec3>& vertices);
    void SetupOpenglBackendData(int32_t width, int32_t height);
    void SetViewport(uint32_t width, uint32_t height);

    template <typename T>
    ErrorCodes_Backend CreateSSBO(SSBOBind<T>& bindData)
    {
        if(bindData.size == 0)
        {
            std::cout << "Unable to create SSBO, bind data is empty!\n";
            return ErrorCodes_Backend::ERROR_SSBO_CREATION;
        }
        glCreateBuffers(1, bindData.ssboId);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, *bindData.ssboId);
        // basically binding ssbo to write data there
        if(bindData.type == 0x00)
            glNamedBufferData(*bindData.ssboId, bindData.size, bindData.data, GL_DYNAMIC_DRAW);
        else 
            glNamedBufferData(*bindData.ssboId, bindData.size, bindData.data, bindData.type);


        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, *bindData.binding, *bindData.ssboId);


        return ErrorCodes_Backend::NO_ERROR;
    }
    template <typename T>
    ErrorCodes_Backend CreateSSBOImmutable(SSBOBind<T>& bindData)
    {
        if(bindData.size == 0)
        {
            std::cout << "Unable to create SSBO, bind data is empty!\n";
            return ErrorCodes_Backend::ERROR_SSBO_CREATION;
        }
        glCreateBuffers(1, bindData.ssboId);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, *bindData.ssboId);
        // basically binding ssbo to write data there
        if(bindData.type == 0x00)
            glNamedBufferStorage(*bindData.ssboId, bindData.size, bindData.data, GL_DYNAMIC_STORAGE_BIT);
        else 
            glNamedBufferStorage(*bindData.ssboId, bindData.size, bindData.data, bindData.type);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, *bindData.binding, *bindData.ssboId);


        return ErrorCodes_Backend::NO_ERROR;
    }
}  




void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei /*length*/, 
    const char* message, const void* /*userParam*/);
GLenum glCheckError_(const char* file, int line);
