#include "../../headers/backend/openglbackend.h"
#include "../../headers/types/types.h"

ErrorCodes_Backend OpenglBackend::CreateSSBO(SSBOBind& bindData)
{
    if(bindData.data == nullptr || bindData.size == 0)
    {
        std::cout << "Unable to create SSBO, bind data is empty!\n";
        return ErrorCodes_Backend::ERROR_SSBO_CREATION;
    }
    glCreateBuffers(1, bindData.ssboId);
    // basically binding ssbo to write data there
    if(bindData.type == 0x00)
        glNamedBufferData(*bindData.ssboId, bindData.size, bindData.data, GL_DYNAMIC_DRAW);
    else 
        glNamedBufferData(*bindData.ssboId, bindData.size, bindData.data, bindData.type);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, *bindData.binding, *bindData.ssboId);


    return ErrorCodes_Backend::NO_ERROR;
}

ErrorCodes_Backend OpenglBackend::CreateSSBO(SSBOBindVec4& bindData)
{
    if(bindData.data == nullptr || bindData.size == 0)
    {
        std::cout << "Unable to create SSBO, bind data is empty!\n";
        return ErrorCodes_Backend::ERROR_SSBO_CREATION;
    }
    glCreateBuffers(1, bindData.ssboId);
    
    // basically binding ssbo to write data there
    if(bindData.type == 0x00)
        glNamedBufferData(*bindData.ssboId, bindData.size, bindData.data, GL_DYNAMIC_DRAW);
    else 
        glNamedBufferData(*bindData.ssboId, bindData.size, bindData.data, bindData.type);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, *bindData.binding, *bindData.ssboId);

    return ErrorCodes_Backend::NO_ERROR;
}

ErrorCodes_Backend OpenglBackend::BindModelVBO(VBOSetupUnskinned& setup)
{
    if(setup.vertices.size() == 0)
    {
        std::cout << "Unable to bind VBO, bind data is empty!\n";
        return ErrorCodes_Backend::ERROR_SSBO_CREATION;
    }
    glGenVertexArrays(1, &setup.VAO);
    glBindVertexArray(setup.VAO);
    glGenBuffers(1, &setup.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, setup.VBO);
    if(setup.type == 0x00)
        glBufferData(GL_ARRAY_BUFFER, setup.vertices.size() * sizeof(glm::vec3), setup.vertices.data(), GL_STATIC_DRAW);
    else
        glBufferData(GL_ARRAY_BUFFER, setup.vertices.size() * sizeof(glm::vec3), setup.vertices.data(), setup.type);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    // to do tangent some day
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return ErrorCodes_Backend::NO_ERROR;
}

ErrorCodes_Backend OpenglBackend::BindModelVBO(VBOSetupUnskinnedVec4& setup)
{
    if(setup.vertices.size() == 0)
    {
        std::cout << "Unable to bind VBO, bind data is empty!\n";
        return ErrorCodes_Backend::ERROR_SSBO_CREATION;
    }
    glCreateVertexArrays(1, &setup.VAO);
    glBindVertexArray(setup.VAO);
    
    glGenBuffers(1, &setup.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, setup.VBO);
    if(setup.type == 0x00)
        glBufferData(GL_ARRAY_BUFFER, setup.vertices.size() * sizeof(glm::vec4), setup.vertices.data(), GL_STATIC_DRAW);
    else
        glBufferData(GL_ARRAY_BUFFER, setup.vertices.size() * sizeof(glm::vec4), setup.vertices.data(), setup.type);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    // to do tangent some day
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return ErrorCodes_Backend::NO_ERROR;
}

ErrorCodes_Backend OpenglBackend::BindModelEBO(EBOSetup& setup)
{
    if(setup.vertices.size() == 0)
    {
        std::cout << "Unable to bind EBO, bind data is empty!\n";
        return ErrorCodes_Backend::ERROR_SSBO_CREATION;
    }
    glGenVertexArrays(1, &setup.VAO);
    glBindVertexArray(setup.VAO);
    glGenBuffers(1, &setup.VBO);
    glGenBuffers(1, &setup.EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, setup.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, setup.indices.size() * sizeof(uint32_t), setup.indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, setup.VBO);
    glBufferData(GL_ARRAY_BUFFER, setup.vertices.size() * sizeof(Vertex), setup.vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    // to do tangent some day
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return ErrorCodes_Backend::NO_ERROR;
}

ErrorCodes_Backend OpenglBackend::BindModelEBO(EBOSetupUnskinned& setup)
{
    if(setup.vertices.size() == 0)
    {
        std::cout << "Unable to bind EBO, bind data is empty!\n";
        return ErrorCodes_Backend::ERROR_SSBO_CREATION;
    }
    glGenVertexArrays(1, &setup.VAO);
    glBindVertexArray(setup.VAO);
    glGenBuffers(1, &setup.VBO);
    glGenBuffers(1, &setup.EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, setup.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, setup.indices.size() * sizeof(uint32_t), setup.indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, setup.VBO);
    glBufferData(GL_ARRAY_BUFFER, setup.vertices.size() * sizeof(glm::vec3), setup.vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return ErrorCodes_Backend::NO_ERROR;

}


void OpenglBackend::SetupOpenglBackendData(int32_t width, int32_t height)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    // glEnable(GL_FRAMEBUFFER_SRGB); // gamma by opengl
    int32_t flags; 
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    // initial viewport
    SetViewport(width, height);
}
void OpenglBackend::SetViewport(uint32_t width, uint32_t height)
{
    glViewport(0,0, width, height);
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei /*length*/, 
    const char* message, const void* /*userParam*/) {
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    std::cout << "---------------\n";
    std::cout << "Debug message (" << id << "): " << message << "\n";
    switch (source){
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    }
    std::cout << "\n";
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    }
    std::cout << "\n";
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
        }    std::cout << "\n\n\n";
}
GLenum glCheckError_(const char* file, int line) {
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")\n";
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)