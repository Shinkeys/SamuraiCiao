#pragma once

#include "../types/types.h"
#include "../assets/assetManager.h"
#include "../systems/camera.h"


#include "../types/collisionTypes.h"
#include "collisionDebug.h"

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
    JPH::BodyManager* _bodyManager = nullptr;
    JPH::PhysicsSystem _physSystem;
    BPLayerInterfaceImpl _broadPhaseLayerInterface;
    JPH::ObjectVsBroadPhaseLayerFilter _objectVsBroadphaseLayerFilter;
    ObjectLayerPairFilterImpl _objectVsObjectLayerFilter;
    
    // event listeners
    MyBodyActivationListener* _bodyActivationListener;
    MyContactListener* _contactListener;
    

    const float _camCylinderRadius = 1.5f;
    Camera* _camera = nullptr;
    AssetManager* _manager = nullptr;
    std::unordered_map<std::string, JPH::Body*> _rigidbodyStorage; 
    std::optional<JPH::Vec3> SimplifyBoxShapes(const std::string& entityName);
    void CreateCollidersForScene();
    void CreateCameraCollider();
    // unordered_map<std::string, BodyID> 
    void PrePhysicsCamUpdate();
    void Setup();

    // debug
    bool _debugCollision{false};
    CollisionDebug _debugInstance;
    // handle to convert shapes to geometry representation for drawing
    using ShapeToGeometryMap = JPH::UnorderedMap<JPH::RefConst<JPH::Shape>, JPH::DebugRenderer::GeometryRef>;
	ShapeToGeometryMap _shapeToGeometry;            
public:
    void WorkWithCollisionDebug();
    void EnableCollisionDisplay();
    void Prepare();
    void Update();
    void PassAssetManager(AssetManager* manager);
    void PassCamera(Camera* camera);
    ~Collision();
};