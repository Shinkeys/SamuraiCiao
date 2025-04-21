#include "../../headers/physics/collisionDebug.h"
#include "../../headers/systems/renderManager.h"

#include "../../headers/systems/shaders.h"
#include "../../headers/systems/camera.h"

CollisionDebug::CollisionDebug()
{
    Shader collisionDebugShader;
    collisionDebugShader.LoadShaders("collisionDebug.vert", "collisionDebug.frag");
    RenderManager::AddShaderByType(std::move(collisionDebugShader), RenderPassType::RENDER_COLLISION_DEBUG);

    glGenVertexArrays(1, &_geometryBuffers.VAO);
    glGenBuffers(1, &_geometryBuffers.VBO);
    glGenBuffers(1, &_geometryBuffers.EBO);

    glGenVertexArrays(1, &_linesBuffers.VAO);
    glGenBuffers(1, &_linesBuffers.VBO);

    DebugRenderer::Initialize();
}


JPH::DebugRenderer::Batch CollisionDebug::CreateTriangleBatch(const JPH::DebugRenderer::Triangle *inTriangles, int inTriangleCount)
{
    TriangleData* triangleData = new TriangleData(inTriangles, inTriangleCount);
    return triangleData;
}


JPH::DebugRenderer::Batch CollisionDebug::CreateTriangleBatch(const JPH::DebugRenderer::Vertex *inVertices, int inVertexCount, 
                                                            const JPH::uint32 *inIndices, int inIndexCount)
{
    TriangleData* triangleData = new TriangleData(inVertices, inVertexCount, inIndices, inIndexCount);
    return triangleData;
}


// Purpose: core method in jolt to draw some shapes, so need to realize that
// Type: own implementation of base jolt method
void CollisionDebug::DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox &inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, 
                                const JPH::DebugRenderer::GeometryRef &inGeometry, JPH::DebugRenderer::ECullMode inCullMode, 
                                JPH::DebugRenderer::ECastShadow inCastShadow, JPH::DebugRenderer::EDrawMode inDrawMode)
{
    // std::cout  << "Draw geometry called\n";

    const JPH::Array<JPH::DebugRenderer::LOD>& geometryLods = inGeometry->mLODs;
    // using LOD 0 as don't use level of details
    TriangleData* triangleBatch = static_cast<TriangleData*>(geometryLods[0].mTriangleBatch.GetPtr());

    const glm::vec3 colliderColor = glm::vec3(static_cast<float>(inModelColor.r) / 256.0f, 
                                    static_cast<float>(inModelColor.g) / 256.0f, 
                                    static_cast<float>(inModelColor.b) / 256.0f);


    if(triangleBatch->_usesIndices)
    {
        auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_COLLISION_DEBUG);
        if(shaderIt == RenderManager::_shaderTypes.end())
        {
            std::cout << "Jolt: Cannot draw geometry, shader not found\n";
            return;
        }

        shaderIt->second.UseShader();
        glBindVertexArray(_geometryBuffers.VAO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _geometryBuffers.EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,  triangleBatch->GetIndices().size() * sizeof(JPH::uint32), triangleBatch->GetIndices().data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, _geometryBuffers.VBO);
        glBufferData(GL_ARRAY_BUFFER, triangleBatch->GetVertices().size() * sizeof(float), triangleBatch->GetVertices().data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glm::mat4 modelMatrix = ConvertJoltMat4ToGlm(inModelMatrix);
        shaderIt->second.SetMat4x4("model", modelMatrix);
        const Matrices& cameraMatrices = SamuraiCameras::g_activeCamera->GetMVP();

        shaderIt->second.SetMat4x4("view", cameraMatrices.view);
        shaderIt->second.SetMat4x4("projection", cameraMatrices.projection);

        shaderIt->second.SetVec3("color", colliderColor);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, triangleBatch->GetIndices().size(), GL_UNSIGNED_INT, triangleBatch->GetIndices().data());
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else
    {
        auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_COLLISION_DEBUG);
        if(shaderIt == RenderManager::_shaderTypes.end())
        {
            std::cout << "Jolt: Cannot draw geometry, shader not found\n";
            return;
        }

        shaderIt->second.UseShader();
        glBindVertexArray(_geometryBuffers.VAO);

        glBindBuffer(GL_ARRAY_BUFFER, _geometryBuffers.VBO);
        glBufferData(GL_ARRAY_BUFFER, triangleBatch->GetVertices().size() * sizeof(float), triangleBatch->GetVertices().data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glm::mat4 modelMatrix = ConvertJoltMat4ToGlm(inModelMatrix);
        shaderIt->second.SetMat4x4("model", modelMatrix);
        const Matrices& cameraMatrices = SamuraiCameras::g_activeCamera->GetMVP();

        shaderIt->second.SetMat4x4("view", cameraMatrices.view);
        shaderIt->second.SetMat4x4("projection", cameraMatrices.projection);

        shaderIt->second.SetVec3("color", colliderColor);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawArrays(GL_TRIANGLES, 0, triangleBatch->_numOfTriangles * 3);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}




// Doesn't need realization of these functions
void CollisionDebug::DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view &inString, JPH::ColorArg inColor, float inHeight)
{
    std::cout << "Draw text 3D call\n";
}

void CollisionDebug::DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, 
                                JPH::ColorArg inColor, JPH::DebugRenderer::ECastShadow inCastShadow)
{
    std::cout << "Draw triangle call\n";
}


void CollisionDebug::DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor)
{
    // std::cout << "Draw line call\n";
    auto shaderIt = RenderManager::_shaderTypes.find(RenderPassType::RENDER_COLLISION_DEBUG);
    if(shaderIt == RenderManager::_shaderTypes.end())
    {
        std::cout << "Jolt: Cannot draw geometry, shader not found\n";
        return;
    }

    const glm::vec3 colliderColor = glm::vec3(static_cast<float>(inColor.r) / 256.0f, 
                                    static_cast<float>(inColor.g) / 256.0f, 
                                    static_cast<float>(inColor.b) / 256.0f);

    constexpr int32_t pointsCount = 2;
    // Doing this conversion because Jolt Vectors are stored like Vec4 but implicitly.
    const std::array<glm::vec3, pointsCount> points { ConvertJoltVec3ToGlm(inFrom), ConvertJoltVec3ToGlm(inTo + inFrom) };

    shaderIt->second.UseShader();
    glBindVertexArray(_linesBuffers.VAO);



    glBindBuffer(GL_ARRAY_BUFFER, _linesBuffers.VBO);
    glBufferData(GL_ARRAY_BUFFER, pointsCount * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    shaderIt->second.SetMat4x4("model", modelMatrix);
    const Matrices& cameraMatrices = SamuraiCameras::g_activeCamera->GetMVP();

    shaderIt->second.SetMat4x4("view", cameraMatrices.view);
    shaderIt->second.SetMat4x4("projection", cameraMatrices.projection);

    shaderIt->second.SetVec3("color", colliderColor);

    glDrawArrays(GL_LINES, 0, pointsCount);

}





