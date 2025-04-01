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


    // Creating object for debug purposes
    _debugInstance = std::make_unique<CollisionDebug>();
    _shapeToGeometry = std::make_unique<ShapeToGeometryMap>();

    // need to initialize it later, so use unique_ptr as it wouldn't see implementation in other case
    _tempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

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
                glm::vec3 displacement = glm::vec3(0.0f);

                auto halfExtent = SimplifyBoxShapes(it->first, displacement);
                if(halfExtent == std::nullopt) 
                    continue;

                Ref<BoxShapeSettings> boxSettings = new BoxShapeSettings(halfExtent.value());

                
                const glm::mat4* objectTransformMatrix = _manager->GetTransformMatrixByName(it->first);
                if(objectTransformMatrix == nullptr)
                {
                    std::cout << "Transform matrix of object " << it->first << " is empty\n";
                    continue;
                }

                const glm::vec3 worldPos = glm::vec3((*objectTransformMatrix)[3]);

                // offset and rotate to dispatch center point
                Ref<RotatedTranslatedShapeSettings> rotBox = new RotatedTranslatedShapeSettings(Vec3(displacement.x, displacement.y, displacement.z),
                                                             Quat::sIdentity(), boxSettings);
                
                BodyCreationSettings rotatedBoxSettings(rotBox, RVec3(0.0f, 0.0f, 0.0f), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
                // creating rigidbody
                Body* body = _bodyInterface->CreateBody(rotatedBoxSettings);
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

std::optional<Vec3> Collision::SimplifyBoxShapes(const std::string& entityName, glm::vec3& inOutDisplacement)
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

        modelAABB.max.x = std::max(modelAABB.max.x, meshVertices.value()[i].position.x);
        modelAABB.max.y = std::max(modelAABB.max.y, meshVertices.value()[i].position.y);
        modelAABB.max.z = std::max(modelAABB.max.z, meshVertices.value()[i].position.z);

    }

    // Transforming to world coords
    const glm::mat4* mat = _manager->GetTransformMatrixByName(entityName);
    if(mat == nullptr)
    {
        std::cout << "Unable to find matrix by name for simplifying mesh for collision\n";
        return std::nullopt;
    }

    // scaling boundaries of the AABB
    modelAABB.min = *mat * glm::vec4(modelAABB.min, 1.0f);
    modelAABB.max = *mat * glm::vec4(modelAABB.max, 1.0f);
    
    // offset to the ground. As object is not guaranteed to be placed on the 0 coordinates,
    // need to calculate offset to displace this obj
    const glm::vec3 midPoint = (modelAABB.max + modelAABB.min) / 2.0f;
    inOutDisplacement = inOutDisplacement + midPoint;
    
    glm::vec3 halfExtent = (modelAABB.max - modelAABB.min) / 2.0f;
    constexpr float offsetToFixZFighting = 0.05f;
    halfExtent += offsetToFixZFighting;

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
    // if(_debugCollision)
    // {
        ShapeToGeometryMap shapeToGeometry;
        BodyIDVector bodies;
        _physSystem.GetBodies(bodies);
        const BodyLockInterface& bli = _physSystem.GetBodyLockInterface();
        for(BodyID body : bodies)
        {
            BodyLockRead lock(bli, body);

            if(lock.SucceededAndIsInBroadPhase())
            {   
                const Body& body = lock.GetBody();
                AllHitCollisionCollector<TransformedShapeCollector> collector;
                body.GetTransformedShape().CollectTransformedShapes(body.GetWorldSpaceBounds(), collector);
                // can allocate memory on heap without deleting because it works like smart ptr
                for(const TransformedShape& transformedShape : collector.mHits)
                {
                    DebugRenderer::GeometryRef geometry;
                    
                    
                    // find if had some geometry in previous frame
                    ShapeToGeometryMap::iterator mapIter = _shapeToGeometry->find(transformedShape.mShape);

                    if(mapIter != _shapeToGeometry->end())
                    {
                        geometry = mapIter->second;
                    }

                    // find geometry from this frame
                    if(geometry == nullptr)
                    {
                        mapIter = shapeToGeometry.find(transformedShape.mShape);
                        if(mapIter != shapeToGeometry.end())
                        {
                            geometry = mapIter->second;
                        }
                    }
                    

                    // else geometry not cached, need to proceed manually
                    if(geometry == nullptr)
                    {
                        Array<DebugRenderer::Triangle> triangles;

                        Shape::GetTrianglesContext context;
                        transformedShape.mShape->GetTrianglesStart(context, AABox::sBiggest(), Vec3::sZero(), Quat::sIdentity(), Vec3::sOne());
                        for(;;)
                        {
                            constexpr int32_t cMaxTriangles = 1000;
                            Float3 vertices[3 * cMaxTriangles];
                            int32_t triangleCount = transformedShape.mShape->GetTrianglesNext(context, cMaxTriangles, vertices);
                            if(triangleCount == 0)
                            {
                                // std::cout << "Can't visualize, triangles count is 0\n";
                                break;
                            }
                            
                            // allocating space
                            size_t outputIndex = triangles.size();
                            triangles.resize(triangles.size() + triangleCount);
                            DebugRenderer::Triangle* triangle = &triangles[outputIndex];
                            
                            for(int32_t vertex = 0, vertexMax = 3 * triangleCount; vertex < vertexMax; vertex += 3, ++triangle)
                            {
                                Vec3 v1(vertices[vertex + 0]);
								Vec3 v2(vertices[vertex + 1]);
								Vec3 v3(vertices[vertex + 2]);

								// Calculate the normal
								Float3 normal;
								(v2 - v1).Cross(v3 - v1).NormalizedOr(Vec3::sZero()).StoreFloat3(&normal);

								v1.StoreFloat3(&triangle->mV[0].mPosition);
								triangle->mV[0].mNormal = normal;
								triangle->mV[0].mColor = Color::sWhite;
								triangle->mV[0].mUV = Float2(0, 0);

								v2.StoreFloat3(&triangle->mV[1].mPosition);
								triangle->mV[1].mNormal = normal;
								triangle->mV[1].mColor = Color::sWhite;
								triangle->mV[1].mUV = Float2(0, 0);

								v3.StoreFloat3(&triangle->mV[2].mPosition);
								triangle->mV[2].mNormal = normal;
								triangle->mV[2].mColor = Color::sWhite;
								triangle->mV[2].mUV = Float2(0, 0);
                            }
                            // to check
                            geometry = new DebugRenderer::Geometry(_debugInstance->CreateTriangleBatch(triangles.data(), triangleCount), transformedShape.mShape->GetLocalBounds());
                            
                        }
                        // caching for the next frame except soft bodies as their shape changes ery frame
                        if(!body.IsSoftBody())
                            shapeToGeometry[transformedShape.mShape] = geometry;


                        Color color;
                        switch(body.GetMotionType())
                        {
                        case EMotionType::Static:
                            color = Color::sPurple;
                            break;
                        case EMotionType::Dynamic:
                            color = Color::sBlue;
                            break;
                        case EMotionType::Kinematic:
                            color = Color::sGreen;
                            break;
                        default:
                            JPH_ASSERT(false);
                            color = Color::sBlack;
                            break;
                        }

                        // drawing
                        Vec3 scale = transformedShape.GetShapeScale();
                        RMat44 matrix = transformedShape.GetCenterOfMassTransform().PreScaled(scale);
                        _debugInstance->DrawGeometry(matrix, AABox::sBiggest(), 1.0f, color, geometry);
                    }
                }       
            }
        }
        _shapeToGeometry.reset(new ShapeToGeometryMap(shapeToGeometry));
    // }

}

void Collision::EnableCollisionDisplay()
{
    ImGui::Checkbox("Debug collision", &_debugCollision);
}