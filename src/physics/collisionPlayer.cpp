#include "../../headers/physics/collisionPlayer.h"
#include "../../headers/systems/interface.h"


using namespace JPH;



void CollisionPlayer::PassPhysSystem(JPH::PhysicsSystem* system)
{
    if(system == nullptr)
    {
        std::cout << "Passed system to the collision player class is null\n";
        return;
    }
    _physSystem = system;
}

void CollisionPlayer::Prepare()
{
    if(_physSystem == nullptr)
    {
        std::cout << "Physics system for collision player is null\n";
        return;
    }
    
    if(_bodyInterface == nullptr)
    {
        _bodyInterface = &_physSystem->GetBodyInterface();
    }

    const glm::vec3& camPos = SamuraiCameras::g_mainCamera.GetPosition();

    const float camHalfHeight = camPos.y / 2.0f;

    ShapeRefC camShape = new CapsuleShape(camHalfHeight, _camCylinderRadius);
    Ref<CharacterVirtualSettings> settings = new CharacterVirtualSettings();
    settings->mMass = 75.0f;
    settings->mShape = camShape;
    settings->mInnerBodyLayer = Layers::MOVING;
    settings->mInnerBodyShape = camShape;
    // everything else is default(at least for now)

    _player = new CharacterVirtual(settings, RVec3(camPos.x, camPos.y, camPos.z), Quat::sIdentity(), 0, _physSystem);
    _player->IsSupported();

    // If would need to use multiple characters in the future:
    // _player->SetCharacterVsCharacterCollision()
}

// void CollisionPlayer::CreateCameraCollider()
// {
//     const glm::vec3& camPos = SamuraiCameras::g_mainCamera.GetPosition();

//     const float camHalfHeight = camPos.y / 2.0f;
    

//     CapsuleShapeSettings cameraCapsuleSettings(camHalfHeight, _camCylinderRadius);
//     cameraCapsuleSettings.SetEmbedded();
    
//     ShapeSettings::ShapeResult cameraShapeRes = cameraCapsuleSettings.Create();
//     if(cameraShapeRes.HasError())
//     {
//         std::cout << "Jolt: "  << cameraShapeRes.GetError() << "\n";
//         return;
//     }
//     ShapeRefC cameraShape = cameraShapeRes.Get();

//     BodyCreationSettings cameraSettings(cameraShape, RVec3(camPos.x, camPos.y, camPos.z), Quat::sIdentity(), EMotionType::Kinematic, Layers::MOVING);

//     // creating rigidbody
//     Body* body = _bodyInterface->CreateBody(cameraSettings);

    
//     if(body == nullptr)
//     {
//         std::cout << "Reached limit of body count, can't create rigidbodies\n";
//     }
    
//     // adding to the world
//     _bodyInterface->AddBody(body->GetID(), EActivation::Activate);
// }

// void CollisionPlayer::PrePhysicsCamUpdate()
// {
//     auto camPos = SamuraiCameras::g_mainCamera.GetPosition();

//     Quat newRotation = Quat::sIdentity();


//     const Vec3 cameraNewPos = Vec3(camPos.x, camPos.y, camPos.z);
//     _bodyInterface->MoveKinematic(_camID, cameraNewPos, newRotation, CollisionDefines::g_DeltaTime);
// }

void CollisionPlayer::PrePhysicsUpdate(TempAllocator* tempAlloc)
{
    CharacterVirtual::ExtendedUpdateSettings updateSettings;
    // TO CHANGE LATER IF WOULD NEED STAIRS OR STICK TO FLOOR
    updateSettings.mWalkStairsStepUp = _player->GetUp() * updateSettings.mWalkStairsStepUp.Length();

    // Update player position
    _player->ExtendedUpdate(CollisionDefines::g_DeltaTime,
    -_player->GetUp() * _physSystem->GetGravity().Length(),
    updateSettings,
    _physSystem->GetDefaultBroadPhaseLayerFilter(Layers::MOVING),
    _physSystem->GetDefaultLayerFilter(Layers::MOVING),
    { }, { },
    *tempAlloc);
}

void CollisionPlayer::HandleInput()
{
    const glm::vec3 movementDir = SamuraiCameras::g_mainCamera.GetMovementDirection();
    const float playerSpeed = SamuraiCameras::g_mainCamera.GetSpeed();
    const glm::vec3 upVector = SamuraiCameras::g_mainCamera.GetUp();
    ResolveInput(ConvertGlmVec3ToJolt(movementDir), ConvertGlmVec3ToJolt(upVector), playerSpeed);
    SamuraiCameras::g_mainCamera.SetPosition(ConvertJoltVec3ToGlm(_player.GetPtr()->GetPosition()));
}


void CollisionPlayer::Update(TempAllocator* tempAlloc)
{
    HandleInput();

    if(tempAlloc == nullptr)
    {
        std::cout << "Temp allocator is null\n";
        return;
    }
    PrePhysicsUpdate(tempAlloc);
}

void CollisionPlayer::ResolveInput(Vec3Arg inMovementDirection, Vec3Arg inUpVector, float inPlayerSpeed)
{
    // bool playerControlsHorizontalVelocity = _player->IsSupported();
    bool playerControlsHorizontalVelocity = true; // Always true now because doesn't need jumps logic as for now

    if(playerControlsHorizontalVelocity)
    {
        // smooth player input
        _velocity = _enablePlayerIntertia?  0.25f * inMovementDirection * inPlayerSpeed + 0.75f * _velocity : inMovementDirection * inPlayerSpeed;

        _allowSliding = !inMovementDirection.IsNearZero();
    }
    else _allowSliding = true; // in air allow to slide

    // Update rotation and up vector
    Quat playerUpRotation = Quat::sEulerAngles(Vec3(inUpVector.GetX(), 0.0f, inUpVector.GetZ()));
    _player->SetUp(playerUpRotation.RotateAxisY());
    _player->SetRotation(playerUpRotation);


    // Update player velocity as it might be changed depends on the platform
    // where  they stays
    _player->UpdateGroundVelocity();

    // Determine new basic velocity
    Vec3 currentVerticalVelocity = _player->GetLinearVelocity().Dot(_player->GetUp()) * _player->GetUp();
    Vec3 groundVelocity = _player->GetGroundVelocity();
    Vec3 newVelocity;

    bool isMovingTowardsGround = (currentVerticalVelocity.GetY() - groundVelocity.GetY()) < 0.1f;
    if(_player->GetGroundState() == CharacterVirtual::EGroundState::OnGround
        && (_enablePlayerIntertia? isMovingTowardsGround                    // Inertia and player is not moving away from ground
        :  !_player->IsSlopeTooSteep(_player->GetGroundNormal())))          // Inertia off and on a slope that not too steep
    {
        // Velocity of ground
        newVelocity = groundVelocity;

        // To do: jump
    }
    else newVelocity = currentVerticalVelocity;

    // Gravity
    newVelocity += (playerUpRotation * _physSystem->GetGravity()) * CollisionDefines::g_DeltaTime;

    if(playerControlsHorizontalVelocity)
    {
        // Player input
        newVelocity += playerUpRotation * _velocity;
    }
    else
    {
        // Preserve horizontal velocity
        Vec3 currentHorizontalVelocity = _player->GetLinearVelocity() - currentVerticalVelocity;
        newVelocity += currentHorizontalVelocity;
    }

    // Update player velocity
    _player->SetLinearVelocity(newVelocity);
    // to do: stance switch
}

void CollisionPlayer::InterfaceUpdate()
{   
    std::array<float, 3> changedVelocity
    {
        0.0f, 0.0f, 0.0f
    };
    ImGui::InputFloat3("Set velocity", changedVelocity.data());
    _velocity.Set(changedVelocity[0], changedVelocity[1], changedVelocity[2]);

    ImGui::Checkbox("Enable inertia", &_enablePlayerIntertia);
}

void CollisionPlayer::Cleanup()
{
    _player->Release();
}