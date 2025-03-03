#pragma once
#include <glad/glad.h>
#include <iostream>
#include "../types/types.h"

namespace OpenglBackend
{
    inline void CreateVAO(uint32_t& vao) { glGenVertexArrays(1, &vao);}
    inline void CreateVBO(uint32_t& vbo) { glGenBuffers(1, &vbo);}
    inline void CreateEBO(uint32_t& ebo) { glGenBuffers(1, &ebo);}
    template <typename T>
    void BindModelVBO(T& setup);
    void BindModelEBO(EBOSetup& setup);
    void BindModelEBO(EBOSetupUnskinned& setup);
    template <typename T>
    void CreateSSBO(T& bindData);
    void SetupOpenglBackendData(int32_t width, int32_t height);
    void SetViewport(uint32_t width, uint32_t height);
}  

template <typename T>
void OpenglBackend::BindModelVBO(T& setup)
{
    glGenVertexArrays(1, &setup.VAO);
    glBindVertexArray(setup.VAO);
    glGenBuffers(1, &setup.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, setup.VBO);
    glBufferData(GL_ARRAY_BUFFER, setup.vertices.size() * sizeof(T), setup.vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(T), (void*)0);
    // to do tangent some day
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template <typename T>
void OpenglBackend::CreateSSBO(T& bindData)
{
    assert(bindData.size > 0 && "Vector size to create SSBO is 0\n");
    glCreateBuffers(1, bindData.ssboId);
    // basically binding ssbo to write data there
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, *bindData.binding, *bindData.ssboId);
    glNamedBufferData(*bindData.ssboId, bindData.size, bindData.data, GL_DYNAMIC_DRAW);

}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei /*length*/, 
    const char* message, const void* /*userParam*/);
GLenum glCheckError_(const char* file, int line);
