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
class MyDebugRenderer;

class Collision
{
private:
    JobSystemThreadPool _jobSystem;
    std::unique_ptr<TempAllocatorImpl> _tempAllocator;
    BodyInterface* _bodyInterface = nullptr;
    BodyManager* _bodyManager = nullptr;
    PhysicsSystem _physSystem;
    BPLayerInterfaceImpl _broadPhaseLayerInterface;
    ObjectVsBroadPhaseLayerFilter _objectVsBroadphaseLayerFilter;
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
    void WorkWithCollisionDebug();
    bool _debugCollision{false};
    CollisionDebug _debugInstance;
public:
    void EnableCollisionDisplay();
    void Prepare();
    void Update();
    void PassAssetManager(AssetManager* manager);
    void PassCamera(Camera* camera);
    ~Collision();
};