#pragma once
#include <glad/glad.h>
#include <iostream>
#include "../types/types.h"

namespace OpenglBackend
{
    inline void CreateVAO(uint32_t& vao) { glGenVertexArrays(1, &vao);}
    inline void CreateVBO(uint32_t& vbo) { glGenBuffers(1, &vbo);}
    inline void CreateEBO(uint32_t& ebo) { glGenBuffers(1, &ebo);}
    void BindUnskinnedObject(std::vector<float> vertices, uint32_t& vao, uint32_t& vbo);
    void BindModelEBO(EBOSetup& setup);
    void SetupOpenglBackendData(int32_t width, int32_t height);
}  

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei /*length*/, 
    const char* message, const void* /*userParam*/);
GLenum glCheckError_(const char* file, int line);
