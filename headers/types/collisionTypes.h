#pragma once
#include "types.h"
// JOLT H MUST BE FIRST
#include <Jolt/Jolt.h>

// Main includes
#include <Jolt/Math/Vec3.h>

#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

// Collision
#include <Jolt/Physics/Collision/ShapeCast.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
// Shapes
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/MeshShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/ScaledShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>
#include <Jolt/Physics/Collision/Shape/ConvexHullShape.h>

namespace CollisionDefines
{
    // simulate physics in 60 fps
    constexpr float g_DeltaTime = 1.0f / 60.0f;
    // 1 collision step per 1/60th sec
    constexpr int32_t g_CollisionSteps = 1;
};


// Purpose: callback for asserts
// Type: Jolt Standard
#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char *inExpression, const char *inMessage, const char *inFile, uint inLine)
{
	// Print to the TTY
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr? inMessage : "") << std::endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS


// Purpose: layers to divide object by types
// Type: Jolt Standard
namespace Layers
{
    static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

// Purpose: class to check which object could collide
// Type: Jolt Standard
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool					ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override;
};

// Purpose: need to divide objects into different broadphases to make them different trees,
// basically like optimization to not update trees of static objects for example
// Type: Jolt Standard
namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr uint32_t NUM_LAYERS(2);
};

// Purpose: This defines a mapping between object and broadphase layers.
// Type: Jolt Standard
class BPLayerInterfaceImpl final : public JPH::BroadPhaseLayerInterface
{
public:
									BPLayerInterfaceImpl()
	{
		// Create a mapping table from object to broad phase layer
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual uint					GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer			GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char *			GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
		default:													JPH_ASSERT(false); return "INVALID";
		}
	}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer					mObjectToBroadPhase[Layers::NUM_LAYERS];
};

// Purpose: class that determines if an object layer can collide with a broadphase layer
// Type: Jolt Standard
class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool				ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override;
};



// Purpose: convert Jolt matrix to GLM matrix
inline glm::mat4 ConvertJoltMat4ToGlm(JPH::RMat44Arg inModelMatrix)
{
	glm::mat4 result;

	JPH::Vec4 axisX = inModelMatrix.GetColumn4(0);
	JPH::Vec4 axisY = inModelMatrix.GetColumn4(1);
	JPH::Vec4 axisZ = inModelMatrix.GetColumn4(2);
	JPH::Vec4 translationPart = inModelMatrix.GetColumn4(3);

	result[0] = glm::vec4(axisX.GetX(), axisX.GetY(), axisX.GetZ(), axisX.GetW());
	result[1] = glm::vec4(axisY.GetX(), axisY.GetY(), axisY.GetZ(), axisY.GetW());
	result[2] = glm::vec4(axisZ.GetX(), axisZ.GetY(), axisZ.GetZ(), axisZ.GetW());
	result[3] = glm::vec4(translationPart.GetX(), translationPart.GetY(), translationPart.GetZ(), translationPart.GetW());

	return result;
}
// Purpose: convert GLM vector to Jolt vector
inline JPH::Vec3 ConvertGlmVec3ToJolt(glm::vec3 inVector)
{
	JPH::Vec3 result;

	result.SetComponent(0, inVector.x);
	result.SetComponent(1, inVector.y);
	result.SetComponent(2, inVector.z);

	return result;
}
// Purpose: convert Jolt vector to GLM vector
inline glm::vec3 ConvertJoltVec3ToGlm(JPH::Vec3 inVector)
{
	glm::vec3 result;

	result.x = inVector.GetX();
	result.y = inVector.GetY();
	result.z = inVector.GetZ();

	return result;
}



struct AABB
{
    glm::vec3 min{glm::vec3(std::numeric_limits<float>::max())};
    glm::vec3 max{glm::vec3(std::numeric_limits<float>::lowest())};
};



// Forward decl
class CollisionDebug;
// Purpose: to inject everything needed from collision class at once
// forward declaration of collision debug;
class CollisionDependency
{
private:
    std::unordered_map<std::string, const JPH::BodyID* const> _objectsHandle;
	JPH::PhysicsSystem& _physSystem;
	CollisionDebug&     _collisionDebug;
public:
	const auto& GetObjectsHandle() const { return _objectsHandle; }
	auto& GetObjectsHandle() 			 { return _objectsHandle; }
	explicit CollisionDependency(JPH::PhysicsSystem& physSystem, CollisionDebug& collisionDebug);
	std::optional<JPH::AABox> GetObjectAABB(const JPH::BodyID& bodyID) const;
	std::optional<std::string> CheckForRayIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection) const;
};
