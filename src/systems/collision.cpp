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

    CollisionData collisionData;

    collisionData.vertices = 
    {
        {min.x, min.y, max.z}, {min.x, min.y, min.z}, {max.x, min.y, max.z}, {max.x, min.y, min.z},
        {min.x, max.y, max.z}, {min.x, max.y, min.z}, {max.x, max.y, max.z}, {max.x, max.y, min.z},
        {min.x, min.y, max.z}, {min.x, max.y, max.z}, {min.x, min.y, max.z}, {max.x, min.y, max.z},
        {max.x, min.y, max.z}, {max.x, max.y, max.z}, {max.x, max.y, max.z}, {min.x, max.y, max.z},
        {min.x, min.y, min.z}, {min.x, max.y, min.z}, {min.x, min.y, min.z}, {max.x, min.y, min.z},
        {max.x, min.y, min.z}, {max.x, max.y, min.z}, {max.x, max.y, min.z}, {min.x, max.y, min.z}
    };

    static uint32_t vertexOffset = 0;
    collisionData.offset = vertexOffset;
    vertexOffset += static_cast<uint32_t>(collisionData.vertices.size());

    _collisionStorage.emplace(aabb.entityName, std::move(collisionData));
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
        modelAABB.entityName = it->first;
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

        if(it->first != "skybox.gltf") 
            AddAABBForModel(modelAABB);

    }
    
    // important: doing traversal like this because if would traverse through collision storage
    // order of data would be messed and would give wrong output
    // placing collision data in buffer to send it to the GPU
    for(auto it = _manager->GetAssetStorage().begin(); it != _manager->GetAssetStorage().end(); ++it)
    {
        auto collisionModel = _collisionStorage.find(it->first);
        
        if(collisionModel != _collisionStorage.end())
        {
            _setup.vertices.insert(_setup.vertices.end(), 
                collisionModel->second.vertices.begin(), collisionModel->second.vertices.end());
        }

        // _setup.indices.insert(_setup.indices.end(), it->second.indices.begin(), it->second.indices.end());
    }




    // sending AABB data to the GPU to make surrouznding box around every model later
    OpenglBackend::BindModelVBO(_setup);


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
        
        for(auto currentMeshIt = _collisionStorage.begin(); currentMeshIt != _collisionStorage.end(); ++currentMeshIt)
        {
            const std::string& meshName = currentMeshIt->first;
            
            const glm::mat4* modelMatrix = _manager->GetTransformMatrixByName(meshName);
            if(modelMatrix != nullptr)
                _debugShader.SetMat4x4("model", *modelMatrix);
            else _debugShader.SetMat4x4("model", glm::mat4(1.0f));

            _debugShader.SetMat4x4("view", view);
            _debugShader.SetMat4x4("projection", projection);
            

            const size_t verticesCount = static_cast<size_t>(currentMeshIt->second.vertices.size());
            glDrawArrays(GL_LINES, currentMeshIt->second.offset, verticesCount);
        }
    // }s
}

void Collision::CheckForCollision(glm::vec3 meshPos)
{
    
}