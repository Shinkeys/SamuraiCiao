#include "../../headers/systems/collision.h"
#include "../../headers/systems/interface.h"
#include "../../headers/backend/openglbackend.h"

using namespace JPH;

// Purpose: callback for traces. Should be in CPP file
// Type: Jolt Standard
static void TraceImpl(const char *inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	std::cout << buffer << std::endl;
}



// Purpose: To make notifications on collision
// Type: Own implementation of Jolt class
class MyContactListener : public ContactListener
{
public:
	// See: ContactListener
	virtual ValidateResult	OnContactValidate(const Body &inBody1, const Body &inBody2, RVec3Arg inBaseOffset, const CollideShapeResult &inCollisionResult) override
	{
		std::cout << "Contact validate callback" << std::endl;

		// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
		return ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void			OnContactAdded(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override
	{
		std::cout << "A contact was added" << std::endl;
	}

	virtual void			OnContactPersisted(const Body &inBody1, const Body &inBody2, const ContactManifold &inManifold, ContactSettings &ioSettings) override
	{
		std::cout << "A contact was persisted" << std::endl;
	}

	virtual void			OnContactRemoved(const SubShapeIDPair &inSubShapePair) override
	{
		std::cout << "A contact was removed" << std::endl;
	}
};
// Purpose: To make notifications on body activation
// Type: Own implementation of Jolt class
class MyBodyActivationListener : public BodyActivationListener
{
public:
	virtual void		OnBodyActivated(const BodyID &inBodyID, uint64 inBodyUserData) override
	{
		std::cout << "A body got activated" << std::endl;
	}

	virtual void		OnBodyDeactivated(const BodyID &inBodyID, uint64 inBodyUserData) override
	{
		std::cout << "A body went to sleep" << std::endl;
	}
};


void Collision::PassAssetManager(AssetManager* manager)
{
    if(manager == nullptr)
    {
        std::cout << "Passed manager to the collision class is null\n";
        return;
    }
    _manager = manager;
}

void Collision::PassCamera(Camera* camera)
{
    if(camera == nullptr)
    {
        std::cout << "Passed camera to the collision class is null\n";
        return;
    }
    _camera = camera;
}


// Purpose: Simplify readability a bit
// Type: Collision class method impl
void Collision::Prepare()
{
    Setup();
    CreateCameraCollider();
    CreateCollidersForScene();
    _physSystem.OptimizeBroadPhase();
}

// Purpose: Initialize all jolt stuff
// Type: Collision class method impl
void Collision::Setup()
{
    if(_manager == nullptr)
    {
        std::cout << "Forgot to pass asset manager to the collision class\n";
        return;
    }

    RegisterDefaultAllocator();

    // installing callbacks
    Trace = TraceImpl;
    JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

    Factory::sInstance = new Factory();

    // register all default physics types
    RegisterTypes();


    // need to initialize it later, so use unique_ptr as it wouldn't see implementation in other case
    _tempAllocator = std::make_unique<JPH::TempAllocatorImpl>(10 * 1024 * 1024);

    _jobSystem.Init(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);
    // rigidbodies amount
    const uint cMaxBodies = 65536;

    // how many mutexes to allocate to protect rigib bodies from concurrency. 0 - def
    const uint cNumBodyMutexes = 0;

    // max amount of body pairs that can be queued at any time
    const uint cMaxBodyPair = 65536;

    // max size of contact buffer if more collision detected, rest would be ignored
    const uint cMaxContactConstraints = 10240;
    // initializing physics system
    _physSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPair, cMaxContactConstraints,
        _broadPhaseLayerInterface, _objectVsBroadphaseLayerFilter, _objectVsObjectLayerFilter);
    
    

    // unique_ptr doesn't work because of static assertions
    _bodyActivationListener = new MyBodyActivationListener;
    _physSystem.SetBodyActivationListener(_bodyActivationListener);
    _contactListener = new MyContactListener;
    _physSystem.SetContactListener(_contactListener);
    
    _bodyManager->Init(cMaxBodies, cNumBodyMutexes, _broadPhaseLayerInterface);
    _bodyInterface = &_physSystem.GetBodyInterface();
}

void Collision::CreateCollidersForScene()
{
    if(_bodyInterface == nullptr)
    {
        std::cout << "Error, can't create colliders as body interface is nullptr\n";
        return;
    }
    for(auto it = _manager->GetAssetStorage().begin(); it  != _manager->GetAssetStorage().end(); ++it)
    {
        switch(it->second.objDesc.type)
        {
        case EntityType::TYPE_BOX_MESH:
            {
                auto boxVertices = SimplifyBoxShapes(it->first);
                if(boxVertices == std::nullopt) 
                    continue;

                BoxShapeSettings boxShapeSettings(Vec3(boxVertices.value())); 
                boxShapeSettings.SetEmbedded();

                ShapeSettings::ShapeResult boxShapeRes = boxShapeSettings.Create();
                if(boxShapeRes.HasError())
                {
                    std::cout << "Jolt: "  << boxShapeRes.GetError() << "\n";
                    continue;
                }
                ShapeRefC boxShape = boxShapeRes.Get();
                
                const glm::mat4* objectTransformMatrix = _manager->GetTransformMatrixByName(it->first);
                if(objectTransformMatrix == nullptr)
                {
                    std::cout << "Transform matrix of object " << it->first << " is empty\n";
                    continue;
                }
                
                const glm::vec3 worldPos = glm::vec3((*objectTransformMatrix)[3]);

                BodyCreationSettings boxSettings(boxShape, RVec3(worldPos.x, worldPos.y, worldPos.z), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);


                // creating rigidbody
                Body* body = _bodyInterface->CreateBody(boxSettings);

                if(body == nullptr)
                {
                    std::cout << "Reached limit of body count, can't create rigidbodies\n";
                    continue;
                }
                _rigidbodyStorage.emplace(it->first, body);
                
                // adding to the world
                _bodyInterface->AddBody(body->GetID(), EActivation::DontActivate);
            

            }
            break;
        }
    }
}

std::optional<Vec3> Collision::SimplifyBoxShapes(const std::string& entityName)
{
    auto meshVertices = _manager->GetMeshVerticesByName(entityName);
    if(meshVertices == std::nullopt)
    {
        std::cout << "Cannot load collision for model " << entityName << '\n';
        return std::nullopt;
    }

    AABB modelAABB;

    for(int32_t i = 0; i < meshVertices.value().size(); ++i)
    {
        modelAABB.min.x = std::min(modelAABB.min.x, meshVertices.value()[i].position.x);
        modelAABB.min.y = std::min(modelAABB.min.y, meshVertices.value()[i].position.y);
        modelAABB.min.z = std::min(modelAABB.min.z, meshVertices.value()[i].position.z);

    }

    // Transforming to world coords
    const glm::mat4* mat = _manager->GetTransformMatrixByName(entityName);
    if(mat == nullptr)
    {
        std::cout << "Unable to find matrix by name for simplifying mesh for collision\n";
        return std::nullopt;
    }

    glm::vec3 halfExtent = (modelAABB.max - modelAABB.min) / 2.0f;

    // Jolt doesn't support 0 convex radius

    // IMPORTANT: IT SHOULD BE MINIMUM!!! 0.05F !!!MINIMUM
    halfExtent.x = std::max(halfExtent.x, 0.05f);
    halfExtent.y = std::max(halfExtent.y, 0.05f);
    halfExtent.z = std::max(halfExtent.z, 0.05f);

    Vec3 halfExtentVec = Vec3(halfExtent.x, halfExtent.y, halfExtent.z);

    return halfExtentVec;
}


void Collision::CreateCameraCollider()
{
    const glm::vec3& camPos = _camera->GetPosition();

    const float camHalfHeight = camPos.y / 2.0f;
    

    CapsuleShapeSettings cameraCapsuleSettings(camHalfHeight, _camCylinderRadius);
    cameraCapsuleSettings.SetEmbedded();
    
    ShapeSettings::ShapeResult cameraShapeRes = cameraCapsuleSettings.Create();
    if(cameraShapeRes.HasError())
    {
        std::cout << "Jolt: "  << cameraShapeRes.GetError() << "\n";
        return;
    }
    ShapeRefC cameraShape = cameraShapeRes.Get();

    BodyCreationSettings cameraSettings(cameraShape, RVec3(camPos.x, camPos.y, camPos.z), Quat::sIdentity(), EMotionType::Kinematic, Layers::MOVING);

    // creating rigidbody
    Body* body = _bodyInterface->CreateBody(cameraSettings);

    if(body == nullptr)
    {
        std::cout << "Reached limit of body count, can't create rigidbodies\n";
    }
    const std::string cameraObjName = "camera";
    _rigidbodyStorage.emplace(cameraObjName, body);
                
    // adding to the world
    _bodyInterface->AddBody(body->GetID(), EActivation::Activate);

}

void Collision::PrePhysicsCamUpdate()
{
    if(_camera == nullptr)
    {
        std::cout << "Camera object is nullptr\n";
        return;
    }
    auto camPos = _camera->GetPosition();
    auto camDir = _camera->GetDirection();

    const float camHalfHeight = camPos.y / 2.0f;
    
    // TO REPLACE TO REPLACE TO REPLACE TO REPLACE TO REPLACE TO REPLACE TO REPLACE 

    CapsuleShapeSettings cameraCapsuleSettings(camHalfHeight, _camCylinderRadius);
    cameraCapsuleSettings.SetEmbedded();
    
    ShapeSettings::ShapeResult cameraShapeRes = cameraCapsuleSettings.Create();
    if(cameraShapeRes.HasError())
    {
        std::cout << "Jolt: "  << cameraShapeRes.GetError() << "\n";
        return;
    }



    RShapeCast shapeCast(cameraShapeRes.Get(), Vec3::sReplicate(1.0f), Mat44::sIdentity(), Vec3(camDir.x, camDir.y, camDir.z));
    ClosestHitCollisionCollector<CastShapeCollector> collector;
    ShapeCastSettings settings;
    _physSystem.GetNarrowPhaseQuery().CastShape(shapeCast, settings, Vec3(0.0f, 0.0f, 0.0f), collector);

    if(collector.HadHit())
    {
        std::cout << "HIT\n";
    }

    ///////////////////////////////////////////////////////////////////

    Quat newRotation = Quat::sIdentity();

    auto cameraObj = _rigidbodyStorage.find("camera");
    if(cameraObj == _rigidbodyStorage.end())
    {
        std::cout << "Camera object is not stored in the storage\n";
        return;
    }


    const Vec3 cameraNewPos = Vec3(camPos.x, camPos.y, camPos.z);
    _bodyInterface->MoveKinematic(cameraObj->second->GetID(), cameraNewPos, newRotation, CollisionDefines::g_DeltaTime);
}


void Collision::Update()
{
    PrePhysicsCamUpdate();
    _physSystem.Update(CollisionDefines::g_DeltaTime, CollisionDefines::g_CollisionSteps, _tempAllocator.get(), &_jobSystem);
}

Collision::~Collision()
{
    for(const auto& body : _rigidbodyStorage)
    {
        _bodyInterface->RemoveBody(body.second->GetID());
        _bodyInterface->DestroyBody(body.second->GetID());
    }


    delete _bodyActivationListener;
    delete _contactListener;

    UnregisterTypes();
    delete Factory::sInstance;
    Factory::sInstance = nullptr;
}

void Collision::WorkWithCollisionDebug()
{
    if(!_debugCollision) return;



}

void Collision::EnableCollisionDisplay()
{
    ImGui::Checkbox("Debug collision", &_debugCollision);
}