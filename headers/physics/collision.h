#pragma once

#include "../types/types.h"
#include "../assets/assetManager.h"

#include "../types/collisionTypes.h"
#include "collisionDebug.h"
#include "collisionPlayer.h"

struct AABB
{
    glm::vec3 min{glm::vec3(std::numeric_limits<float>::max())};
    glm::vec3 max{glm::vec3(std::numeric_limits<float>::lowest())};
};

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
    // unordered_map<std::string, BodyID> 

    // Debug
    bool _debugCollision{false};
    std::unique_ptr<CollisionDebug> _debugInstance;
    // handle to convert shapes to geometry representation for drawing
    using ShapeToGeometryMap = JPH::UnorderedMap<JPH::RefConst<JPH::Shape>, JPH::DebugRenderer::GeometryRef>;
	std::unique_ptr<ShapeToGeometryMap> _shapeToGeometry;
    
    // Player(need to be ptr with later init)
    CollisionPlayer _playerCollision;

    void PassStructures();

    CollisionDependency _dependencies;
public:
    JPH::PhysicsSystem* GetPhysSystem() { return &_physSystem; }
    void WorkWithCollisionDebug();
    void InterfaceUpdate();
    const CollisionDependency& GetDependencies() const { return _dependencies;}
    void Prepare();
    void Update();
    void PassAssetManager(AssetManager* manager);
    ~Collision();
};