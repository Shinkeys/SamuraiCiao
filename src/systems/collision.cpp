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
        {min.x, min.y, max.z}, // left bottom corner front
        {max.x, min.y, max.z}, // right bottom corner front
        {max.x, max.y, max.z}, // right top corner front
        {min.x, max.y, max.z}, // left top corner front
        {min.x, min.y, min.z}, // left bottom corner back
        {max.x, min.y, min.z}, // right bottom corner back
        {max.x, max.y, min.z}, // right top corner back
        {min.x, max.y, min.z}, // left top corner back
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
    std::for_each(aabbIndices.begin(), aabbIndices.end(), [&](uint32_t& value)
    {
        value += indicesOffset;
    });

    indicesOffset += static_cast<uint32_t>(aabbIndices.size());

    _setup.indices.reserve(aabbIndices.size());
    _setup.indices.insert(_setup.indices.end(), aabbIndices.begin(), aabbIndices.end());
}

void Collision::Prepare()
{
    _debugShader.LoadShaders("collisionDebug.vert", "collisionDebug.frag");


    // calculating minimal and maximal points of model to make AABB

    for(auto it = _manager->GetAssetStorage().begin(); it != _manager->GetAssetStorage().end(); ++it)
    {
        auto meshVertices = _manager->GetMeshVerticesByName(it->first);
        if(meshVertices == std::nullopt)
        {
            std::cout << "Cannot load collision for model " << it->first << '\n';
            break;
        }
        
        AABB modelAABB;
        for(int32_t i = 0; i < meshVertices.value().size(); ++i)
        {
            if(it->first == "skybox.gltf") break;
            // calculating min points of mesh
            modelAABB.min.x = std::min(modelAABB.min.x, meshVertices.value()[i].position.x);
            modelAABB.min.y = std::min(modelAABB.min.y, meshVertices.value()[i].position.y);
            modelAABB.min.z = std::min(modelAABB.min.z, meshVertices.value()[i].position.z);
    
                // calculating max points of mesh
            modelAABB.max.x = std::max(modelAABB.max.x, meshVertices.value()[i].position.x);
            modelAABB.max.y = std::max(modelAABB.max.y, meshVertices.value()[i].position.y);
            modelAABB.max.z = std::max(modelAABB.max.z, meshVertices.value()[i].position.z);
        }

        AddAABBForModel(modelAABB);
    }
    

    // sending AABB data to the GPU to make surrouznding box around every model later
    OpenglBackend::BindModelEBO(_setup);

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

        const int32_t countOfIndices = 24;
        uint32_t offset = 0;
        glBindVertexArray(_setup.VAO);
        _debugShader.UseShader();
        for(const auto& currentMesh : _manager->GetAssetStorage())
        {
            const std::string& meshName = currentMesh.first;
            if(meshName == "skybox.gltf") continue;
            
            const glm::mat4* modelMatrix = _manager->GetTransformMatrixByName(meshName);

            _debugShader.SetMat4x4("model", *modelMatrix);
            _debugShader.SetMat4x4("view", view);
            _debugShader.SetMat4x4("projection", projection);
                
            glDrawElements(GL_LINES, countOfIndices, GL_UNSIGNED_INT, (void*)(offset + _setup.indices.data()));
            offset += static_cast<uint32_t>(countOfIndices);
            
        }
    // }
}

void Collision::CheckForCollision(glm::vec3 meshPos)
{
    
}