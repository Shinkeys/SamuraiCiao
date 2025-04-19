#pragma once

#include "../types/types.h"
#include "../assets/assetManager.h"

#include "../types/collisionTypes.h"
#include "collisionDebug.h"
#include "collisionPlayer.h"


class MyBodyActivationListener; 
class MyContactListener;

class Collision
{
private:
    JPH::JobSystemThreadPool _jobSystem;
    std::unique_ptr<JPH::TempAllocatorImpl> _tempAllocator;
    JPH::BodyInterface* _bodyInterface = nullptr;
    JPH::PhysicsSystem _physSystem;
    BPLayerInterfaceImpl _broadPhaseLayerInterface;
    JPH::ObjectVsBroadPhaseLayerFilter _objectVsBroadphaseLayerFilter;
    ObjectLayerPairFilterImpl _objectVsObjectLayerFilter;
    
    // event listeners
    MyBodyActivationListener* _bodyActivationListener;
    MyContactListener* _contactListener;
    

    AssetManager* _manager = nullptr;
    std::unordered_map<std::string, JPH::Body*> _rigidbodyStorage; 
    std::optional<JPH::Vec3> SimplifyBoxShapes(const std::string& entityName, glm::vec3& inOutDisplacement);
    void CreateCollidersForScene();
    void Setup();

    // Debug
    bool _debugCollision{false};
    std::unique_ptr<CollisionDebug> _debugInstance;
    // handle to convert shapes to geometry representation for drawing
    using ShapeToGeometryMap = JPH::UnorderedMap<JPH::RefConst<JPH::Shape>, JPH::DebugRenderer::GeometryRef>;
	std::unique_ptr<ShapeToGeometryMap> _shapeToGeometry;
    
    // Player(need to be ptr with later init)
    CollisionPlayer _playerCollision;

    void PassStructures();

    std::unique_ptr<CollisionDependency> _dependencies;

    std::optional<AABB> CalculateMeshAABB(const std::string& entityName);
public:
    CollisionDependency* GetCollisionDependency() { return _dependencies.get(); }
    JPH::PhysicsSystem* GetPhysSystem() { return &_physSystem; }
    void WorkWithCollisionDebug();
    void InterfaceUpdate();
    void Prepare();
    void Update();
    void PassAssetManager(AssetManager* manager);

    // Collision(Collision&) = delete;
    // TO DO
    // Collision(Collision&& oldInstance)
    // {
    //     if(this != &oldInstance)
    //     {
            
    //     }
    // }
    ~Collision();
};