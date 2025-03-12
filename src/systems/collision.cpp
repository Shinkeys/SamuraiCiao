#include "../../headers/systems/collision.h"
#include "../../headers/systems/interface.h"
#include "../../headers/backend/openglbackend.h"

void Collision::PassAssetManager(AssetManager& manager)
{
    _manager = &manager;
}


void Collision::AddAABBForModel(const AABB& aabb)
{
    const glm::vec3& min = aabb.min;
    const glm::vec3& max = aabb.max;

    std::vector<glm::vec3> aabbVert
    {
        {min.x, min.y, min.z}, // left bottom corner front
        {max.x, min.y, min.z}, // right bottom corner front
        {max.x, max.y, min.z}, // right top corner front
        {min.x, max.y, min.z}, // left top corner front
        {min.x, min.y, max.z}, // left bottom corner back
        {max.x, min.y, max.z}, // right bottom corner back
        {max.x, max.y, max.z}, // right top corner back
        {min.x, max.y, max.z}, // left top corner back
    };

    _setup.vertices.reserve(aabbVert.size());
    _setup.vertices.insert(_setup.vertices.end(), aabbVert.begin(), aabbVert.end());
    
    static uint32_t indicesOffset = 0;
    std::vector<uint32_t> aabbIndices
    {
        0, 4, 1, 5,  3, 7, 2, 6, // connecting front and back
        0, 3, 0, 1,  1, 2, 2, 3, // front with each other
        4, 7, 4, 5,  5, 6, 6, 7  // back  with each other
    };


    // here for readability
    for(int32_t i = 0; i < aabbIndices.size(); ++i)
    {
        aabbIndices[i] += indicesOffset;
    }
    _setup.indices.reserve(aabbIndices.size());
    _setup.indices.insert(_setup.indices.end(), aabbIndices.begin(), aabbIndices.end());
}

void Collision::Prepare()
{
    _debugShader.LoadShaders("collisionDebug.vert", "collisionDebug.frag");

    // calculating minimal and maximal points of model to make AABB
    const auto& meshesDesc = _manager->GetAssetStorage();
    int32_t indexOfOffset = 0;
    for(auto it = meshesDesc.begin(); it != meshesDesc.end(); ++it)
    {
        AABB modelAABB;

        const std::vector<Vertex>&  allMeshesBuffer = _manager->GetBuffers().vertices;
        for(int32_t j = 0; j < it->second.currMeshVertCount.size(); ++j)
        {
            const int32_t index = j + it->second.meshIndexOffset[indexOfOffset];
            
            // calculating min points of mesh
            modelAABB.min.x = std::min(modelAABB.min.x, allMeshesBuffer[index].position.x);
            modelAABB.min.y = std::min(modelAABB.min.y, allMeshesBuffer[index].position.y);
            modelAABB.min.z = std::min(modelAABB.min.z, allMeshesBuffer[index].position.z);

            // calculating max points of mesh
            modelAABB.max.x = std::max(modelAABB.max.x, allMeshesBuffer[index].position.x);
            modelAABB.max.y = std::max(modelAABB.max.y, allMeshesBuffer[index].position.y);
            modelAABB.max.z = std::max(modelAABB.max.z, allMeshesBuffer[index].position.z);
        }
        AddAABBForModel(modelAABB);
        ++indexOfOffset;
    }


    OpenglBackend::BindModelEBO(_setup);

    // sending AABB data to the GPU to make surrounding box around every model later
}


void Collision::UpdateAABB()
{
    // traverse mesh vertices find low left corner, top right corner
}

// passing as parameters because would refactor matrices system later
void Collision::VisualizeAABB(const glm::mat4& view, const glm::mat4& projection)
{
    // static bool visualizeAABB = false;
    // ImGui::Checkbox("Show collision AABB", &visualizeAABB);

    // if(visualizeAABB)
    // {
        glBindVertexArray(_setup.VAO);
        _debugShader.UseShader();
        for(const auto& currentMesh : _manager->GetAssetStorage())
        {
            const std::string& meshName = currentMesh.first;
            if(meshName != "skybox.gltf")
            {
                const glm::mat4* modelMatrix = _manager->GetTransformMatrixByName(meshName);

                _debugShader.SetMat4x4("model", *modelMatrix);
                _debugShader.SetMat4x4("view", view);
                _debugShader.SetMat4x4("projection", projection);

                glDrawElements(GL_LINES, _setup.indices.size(), GL_UNSIGNED_INT, _setup.indices.data());
            }
        }
    // }
}

void Collision::CheckForCollision(glm::vec3 meshPos)
{
    
}