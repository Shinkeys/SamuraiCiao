#include "../../headers/systems/collision.h"
#include "../../headers/systems/interface.h"
#include "../../headers/backend/openglbackend.h"


void Collision::PassAssetManager(AssetManager& manager)
{
    _manager = &manager;
}


void Collision::AddCollisionTypeToObject(std::string entityName, CollisionType type)
{
    if(type == CollisionType::COLLISION_AABB)
    {
        _collisionTypesStorage[entityName] = type;
    }
    else if(type == CollisionType::COLLISION_SPHERE)
    {
        _collisionTypesStorage[entityName] = type;
    }
}   

void Collision::CalculateModelSphere(const std::vector<Vertex>& vertices, const std::string& entityName, float radius)
{
    Sphere modelSphere;
    modelSphere.entityName = entityName;

    glm::vec3 minPoint{glm::vec3(std::numeric_limits<float>::max())};
    glm::vec3 maxPoint{glm::vec3(std::numeric_limits<float>::lowest())};
    for(int32_t i = 0; i < vertices.size(); ++i)
    {
        // calculating min points of mesh
        minPoint.x = std::min(minPoint.x, vertices[i].position.x);
        minPoint.y = std::min(minPoint.y, vertices[i].position.y);
        minPoint.z = std::min(minPoint.z, vertices[i].position.z);
            
        // calculating max points of mesh
        maxPoint.x = std::max(maxPoint.x, vertices[i].position.x);
        maxPoint.y = std::max(maxPoint.y, vertices[i].position.y);
        maxPoint.z = std::max(maxPoint.z, vertices[i].position.z);
    }

    modelSphere.center = (minPoint + maxPoint) / 2.0f;
    modelSphere.radius = radius;

    

}

void Collision::CalculateModelAABB(const std::vector<Vertex>& vertices, const std::string& entityName)
{
    AABB modelAABB;
    modelAABB.entityName = entityName;
    
    for(int32_t i = 0; i < vertices.size(); ++i)
    {
        // calculating min points of mesh

        modelAABB.min.x = std::min(modelAABB.min.x, vertices[i].position.x);
        modelAABB.min.y = std::min(modelAABB.min.y, vertices[i].position.y);
        modelAABB.min.z = std::min(modelAABB.min.z, vertices[i].position.z);
            

        // calculating max points of mesh
        modelAABB.max.x = std::max(modelAABB.max.x, vertices[i].position.x);
        modelAABB.max.y = std::max(modelAABB.max.y, vertices[i].position.y);
        modelAABB.max.z = std::max(modelAABB.max.z, vertices[i].position.z);

    }
    AddAABBForModel(modelAABB);
}

void Collision::AddAABBForModel(const AABB& aabb)
{
    const glm::vec3& min = aabb.min;
    const glm::vec3& max = aabb.max;

    AABBData collisionData;

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

    // storing max and min points of AABB box
    collisionData.aabbPoints = aabb;

    _collisionAABBStorage.emplace(aabb.entityName, std::move(collisionData));
}

void Collision::Prepare()
{
    _debugShader.LoadShaders("collisionDebug.vert", "collisionDebug.frag");


    // calculating minimal and maximal points of model to make AABB

    for(auto it = _manager->GetAssetStorage().begin(); it != _manager->GetAssetStorage().end(); ++it)
    {
        // skipping object if object type is not a mesh
        if(it->second.objDesc.type != EntityType::TYPE_MESH) continue;

        auto meshVertices = _manager->GetMeshVerticesByName(it->first);
        if(meshVertices == std::nullopt)
        {
            std::cout << "Cannot load collision for model " << it->first << '\n';
            break;
        }
        
        const std::string& entityName = it->first;

        // looking for type of collision for object, if not found then AABB default
        auto meshType = _collisionTypesStorage.find(entityName);
        if(meshType == _collisionTypesStorage.end())
        {
            CalculateModelAABB(meshVertices.value(), entityName);
            continue;
        }


        switch(meshType->second)
        {
        case CollisionType::COLLISION_AABB:
            CalculateModelAABB(meshVertices.value(), entityName);
            break;

        case CollisionType::COLLISION_SPHERE:
            CalculateModelSphere(meshVertices.value(), entityName, 1.0f);
            break;

        default: 
            std::cout << "Error in calculating collision for object, called default statement in prepare\n";
            break;
        }

    }
    
    // important: doing traversal like this because if would traverse through collision storage
    // order of data would be messed and would give wrong output
    // placing collision data in buffer to send it to the GPU
    for(auto it = _manager->GetAssetStorage().begin(); it != _manager->GetAssetStorage().end(); ++it)
    {
        CollisionType objectType = CollisionType::COLLISION_AABB;
        const std::string& entityName = it->first;

        auto findObjectType = _collisionTypesStorage.find(entityName);
        if(findObjectType != _collisionTypesStorage.end())
            objectType = findObjectType->second;


        switch(objectType)
        {
        case CollisionType::COLLISION_AABB:
            {
                auto collisionModel = _collisionAABBStorage.find(entityName);
            
                if(collisionModel != _collisionAABBStorage.end())
                {
                    _setup.vertices.insert(_setup.vertices.end(), 
                    collisionModel->second.vertices.begin(), collisionModel->second.vertices.end());
                }
                break;
            }
        
        // case CollisionType::COLLISION_SPHERE:
        //     {
        //         auto collisionModel = _collisionSphereStorage.find(entityName);
        
        //         if(collisionModel != _collisionSphereStorage.end())
        //         {
        //             _setup.vertices.insert(_setup.vertices.end(), 
        //                 collisionModel->second.vertices.begin(), collisionModel->second.vertices.end());
        //         }
        //         break;
        //     }

        default: 
            std::cout << "Unable to add vertices for object collision, not found by type\n";
            break;
        }

        // _setup.indices.insert(_setup.indices.end(), it->second.indices.begin(), it->second.indices.end());
    }




    // sending AABB data to the GPU to make surrouznding box around every model later
    OpenglBackend::BindModelVBO(_setup);

}

void Collision::EnableCollisionDisplay()
{
    ImGui::Checkbox("Show collision AABB", &_visualizeAABB);
}

void Collision::UpdateAABB()
{
    // traverse mesh vertices find low left corner, top right corner
}

// passing as parameters because would refactor matrices system later
void Collision::VisualizeAABB(const glm::mat4& view, const glm::mat4& projection)
{
    if(_visualizeAABB)
    {
        glBindVertexArray(_setup.VAO);
        _debugShader.UseShader();
        
        for(auto currentMeshIt = _collisionAABBStorage.begin(); currentMeshIt != _collisionAABBStorage.end(); ++currentMeshIt)
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
    }
}

void Collision::CheckCameraForCollision(Camera& camera)
{
    for(auto currentMeshIt = _collisionAABBStorage.begin(); currentMeshIt != _collisionAABBStorage.end(); ++currentMeshIt)
    {
        const std::string& meshName = currentMeshIt->first;
        
        const glm::mat4* modelMatrix = _manager->GetTransformMatrixByName(meshName);
        if(modelMatrix == nullptr)
        {
            std::cout << "Can't calculate collision with the camera, model matrix" << meshName << "is nullptr\n";
            continue;
        }
        glm::vec4 transformWorldSpaceCam = glm::mat4(1.0f) * glm::vec4(camera.GetPosition(), 1.0f);
        const glm::vec3 cameraWorldPos = static_cast<glm::vec3>(transformWorldSpaceCam);
        
        glm::vec4 transformWorldSpaceAABBMin = *modelMatrix * glm::vec4(currentMeshIt->second.aabbPoints.min, 1.0f);
        const glm::vec3 aabbMinWorldPos = static_cast<glm::vec3>(transformWorldSpaceAABBMin) / transformWorldSpaceAABBMin.w;
        glm::vec4 transformWorldSpaceAABBMax = *modelMatrix * glm::vec4(currentMeshIt->second.aabbPoints.max, 1.0f);
        const glm::vec3 aabbMaxWorldPos = static_cast<glm::vec3>(transformWorldSpaceAABBMax) / transformWorldSpaceAABBMax.w;


        const glm::vec3 aabbWorldCenter = (aabbMaxWorldPos + aabbMinWorldPos) / 2.0f;
        // specifying size of mesh
        const glm::vec3 objectWorldExtent = aabbMaxWorldPos - aabbMinWorldPos;

        static int coll = 0;
        if(CalculateCameraCollision(camera, aabbWorldCenter, objectWorldExtent))
        {
            std::cout << coll++ << "Collision\n";
        }
        
    }
}

// Purpose: camera collision would be represented via sphere type, so would calculate collision with the others
// according to this fact
// Idea: finding closest point on the OBJECT and checking if some parts inside of the camera sphere. If so - collision detected
bool Collision::CalculateCameraCollision(Camera& camera, const glm::vec3& objectCenter, const glm::vec3& objectExtent)
{  
    const glm::vec3& camPos = camera.GetPosition();
    // distance from sphere center(camera) to the object
    const glm::vec3 distance = camPos - objectCenter;

    const glm::vec3 halfExtent = objectExtent / 2.0f;

    const glm::vec3 nearestPoint = objectCenter + glm::clamp(distance, -halfExtent, halfExtent);

    const glm::vec3 difference = camPos - nearestPoint;

    return glm::length(difference) < _cameraSphereRadius;
}
