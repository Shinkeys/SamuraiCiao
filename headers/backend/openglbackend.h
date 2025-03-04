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
    ErrorCodes_Backend CreateSSBO(SSBOBind& bindData);
    ErrorCodes_Backend CreateSSBO(SSBOBindVec4& bindData);
    void SetupOpenglBackendData(int32_t width, int32_t height);
    void SetViewport(uint32_t width, uint32_t height);
}  




void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei /*length*/, 
    const char* message, const void* /*userParam*/);
GLenum glCheckError_(const char* file, int line);
