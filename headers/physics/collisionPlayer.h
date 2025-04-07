#pragma once
#include "../types/collisionTypes.h"

#include <Jolt/Physics/Character/CharacterVirtual.h>

#include "../systems/camera.h"




// Purpose: as I need to handle collision of camera(basically player)
// with the environment, would realize this class to handle this behaviour
// Type: own implementation with the usage of Jolt base class
class CollisionPlayer : JPH::CharacterContactListener
{
private:
    // virtual void						OnContactAdded(const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2, 
    //                                             const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition, 
    //                                             JPH::Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) override;
    // virtual void						OnAdjustBodyVelocity(const JPH::CharacterVirtual *inCharacter, const JPH::Body &inBody2, 
    //                                                         JPH::Vec3 &ioLinearVelocity, JPH::Vec3 &ioAngularVelocity) override;
	// virtual void						OnContactPersisted(const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2, 
    //                                                     const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::
    //                                                     Vec3Arg inContactNormal, JPH::CharacterContactSettings &ioSettings) override;
    // virtual void						OnContactRemoved(const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2, 
    //                                                     const JPH::SubShapeID &inSubShapeID2) override;
    // virtual void						OnCharacterContactAdded(const JPH::CharacterVirtual *inCharacter, const JPH::CharacterVirtual *inOtherCharacter, 
    //                                                             const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, 
    //                                                             JPH::CharacterContactSettings &ioSettings) override;                                    
    // virtual void						OnCharacterContactPersisted(const JPH::CharacterVirtual *inCharacter, const JPH::CharacterVirtual *inOtherCharacter, 
    //                                                                 const JPH::SubShapeID &inSubShapeID2, JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, 
    //                                                                 JPH::CharacterContactSettings &ioSettings) override;
    // virtual void						OnCharacterContactRemoved(const JPH::CharacterVirtual *inCharacter, const JPH::CharacterID &inOtherCharacterID, 
    //                                                             const JPH::SubShapeID &inSubShapeID2) override;
    // virtual void						OnContactSolve(const JPH::CharacterVirtual *inCharacter, const JPH::BodyID &inBodyID2, const JPH::SubShapeID &inSubShapeID2, 
    //                                                 JPH::RVec3Arg inContactPosition, JPH::Vec3Arg inContactNormal, JPH::Vec3Arg inContactVelocity, 
    //                                                 const JPH::PhysicsMaterial *inContactMaterial, JPH::Vec3Arg inCharacterVelocity, 
    //                                                 JPH::Vec3 &ioNewCharacterVelocity) override;

    
    // Camera settings
    const float _camCylinderRadius = 1.0f;
    
    // void PrePhysicsCamUpdate();
    void CreateCameraCollider();
    void ResolveInput(JPH::Vec3Arg inMovementDirection, bool inJump, JPH::Vec3Arg inUpVector, float inPlayerSpeed);
    void HandleInput();
    void PrePhysicsUpdate(JPH::TempAllocator* tempAlloc);
    
    // Common
    JPH::Vec3 _velocity = JPH::Vec3::sZero();
    bool _allowSliding         { false };
    bool _enablePlayerIntertia { true };
    bool _canMoveDuringJump    { true };
    float _jumpSpeed = 7.5f;
    float _jumpMultiplier = 1.15f;
    JPH::PhysicsSystem* _physSystem    = nullptr;
    JPH::BodyInterface* _bodyInterface = nullptr;

    JPH::Ref<JPH::CharacterVirtual> _player;
public:
    void Prepare();
    void PassPhysSystem(JPH::PhysicsSystem* system);

    // physics update
    void Update(JPH::TempAllocator* tempAlloc);
    // Interface
    void InterfaceUpdate();



    void Cleanup();
};