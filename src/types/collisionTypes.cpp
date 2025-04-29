#include "../../headers/types/collisionTypes.h"
#include "../../headers/math/math.h"


CollisionDependency::CollisionDependency(JPH::PhysicsSystem& physSystem) : _physSystem{physSystem}
{

}

CollisionCreateDesc::CollisionCreateDesc()
{

}

CollisionCreateDesc::CollisionCreateDesc(const std::string& name, glm::vec3 axis, glm::vec3 position, float angle, float halfCylinderExtent, float radius) :
                                        _name{name}, _axis{axis}, _position{position}, _angle{angle}, _halfCylinderExtent{halfCylinderExtent}, _radius{radius}
{

}


std::optional<JPH::AABox> CollisionDependency::GetObjectAABB(const JPH::BodyID& bodyID) const
{
    using namespace JPH;

    BodyIDVector bodies;
    const BodyLockInterface& bli = _physSystem.GetBodyLockInterface();
    BodyLockRead lock(bli, bodyID);

    if(lock.Succeeded())
    {   
        const Body& body = lock.GetBody();
        const Shape* shape = body.GetShape();
        if(shape == nullptr)
        {
            std::cout << "Can't get AABB of object with id " << bodyID.GetIndex() << " to prepare object selection\n";
            return std::nullopt;
        }   
            
        // Doing this to return body in space of passed vertices(currently world space)
        JPH::AABox localBounds = shape->GetLocalBounds();
        localBounds.Translate(shape->GetCenterOfMass());
        
        return localBounds;
    }


    std::cout << "Requested mesh AABB is unavailable(Probably incorrect ID or body lock issue\n";
    return std::nullopt;
}


// Purpose: method to check for ray intersection with every object in scene(added to the physics system)
// Return type: if not found std::nullopt, otherwise object's name.
std::optional<std::string> CollisionDependency::CheckForRayIntersection(glm::vec3 rayOrigin, glm::vec3 rayDirection) const
{
    if(_objectsHandle.empty())
    {
        std::cout << "Can't check ray for intersection, objects handle is null\n";
        return std::nullopt;
    }

    float minDistance = std::numeric_limits<float>::max();
    std::string objectName;


    for(const auto& nameID : _objectsHandle)
    {
        if(nameID.second == nullptr)
        {
            std::cout << "Object's: " << nameID.first << " ID is nullptr\n";
            continue;
        }
        auto optAABB = GetObjectAABB(*nameID.second);
        if(optAABB == std::nullopt)
        {
            std::cout << "Object's: " << nameID.first << " AABB is empty\n";
            continue;
        }

        std::cout << nameID.first << "\n";

        JPH::AABox objectAABB = optAABB.value();
        
        if(SamuraiMath::IntersectAABB(rayOrigin, rayDirection, 
            ConvertJoltVec3ToGlm(objectAABB.mMin), ConvertJoltVec3ToGlm(objectAABB.mMax)))
        {
            std::cout << "Intersects\n";

            const JPH::Vec3 objectAABoxCenter = (objectAABB.mMin + objectAABB.mMax) / 2.0f;
            const float currDistance = glm::distance(rayOrigin, ConvertJoltVec3ToGlm(objectAABoxCenter));
            
            if(currDistance < minDistance)
            {
                objectName  = nameID.first;
                minDistance = currDistance;
            }
        }
    }

    if(objectName.empty())
        return std::nullopt;

    return objectName;
}

void CollisionDependency::MoveCollider(const std::string& entityName, glm::vec3 newPos)
{
    if(_objectsHandle.find(entityName) == _objectsHandle.end())
    {
        std::cout << "Unable to move collider, object " << entityName << " is not found in the storage\n";
        return;
    }

    const JPH::BodyID* bodyID = _objectsHandle[entityName];
    if(bodyID == nullptr)
    {
        std::cout << "Unable to move collider, bodyID is nullptr for object " << entityName << '\n';
        return;
    }
    _physSystem.GetBodyInterface().MoveKinematic(*bodyID, ConvertGlmVec3ToJolt(newPos), JPH::Quat::sIdentity(), CollisionDefines::g_DeltaTime);
}

void CollisionDependency::UpdateLineData(const LineDebug& newData)
{
    _clickRay = newData;
    _clickRay.changed = true;

    if(_clickRay.endType == LineDebug::EndType::END_TYPE_DIRECTION)
    {
        _clickRay.direction *= 1000.0f;
    }
}

void CollisionDependency::AddCommand(CollisionCmd command) 
{
    _commandsQueue.push(command);
}
void CollisionDependency::AddCommand(CollisionCmd command, CollisionCreateDesc& createDesc)
{
    _commandsQueue.push(command);
    _creationDescQueue.push(createDesc);
}

void CollisionDependency::RemoveCommand()
{
    if(_commandsQueue.empty())
    {
        std::cout << "Can't remove command from the queue, it's empty\n";
        return;
    }

    switch(_commandsQueue.front())
    {
    case CollisionCmd::COLLISION_CREATE_BOX:
        _creationDescQueue.pop();
        break;
    case CollisionCmd::COLLISION_CREATE_CAPSULE:
        _creationDescQueue.pop();
        break;
    }

    _commandsQueue.pop();
}

CollisionCmd CollisionDependency::GetNextCommand() const
{
    if(_commandsQueue.empty())
    {
        // std::cout << "Can't get next command from the queue, it's empty\n";
        // Returning default command(no commands in the list) if queue is empty, to handle in the
        // collision class.
        return CollisionCmd::COLLISION_CMD_EMPTY;
    }

    return _commandsQueue.front();
}
CollisionCreateDesc& CollisionDependency::GetCreationDesc()
{
    if(_creationDescQueue.empty())
    {
        std::cout << "You forgot to pass creation data\n";
        _creationDescQueue.push(CollisionCreateDesc{});
    }

    return _creationDescQueue.front();
}

CapsuleCreateInfo CollisionCreateDesc::GetCapsuleDesc()
{
    CapsuleCreateInfo createInfo;
    createInfo.name = _name;
    createInfo.axis = _axis;
    createInfo.angle = _angle;
    createInfo.position = _position;
    createInfo.halfCylinderExtent = _halfCylinderExtent;
    createInfo.radius = _radius;


    return createInfo;
}

/*
  _        _ __   _______ ____  ____  
 | |      / \\ \ / / ____|  _ \/ ___| 
 | |     / _ \\ V /|  _| | |_) \___ \ 
 | |___ / ___ \| | | |___|  _ < ___) |
 |_____/_/   \_\_| |_____|_| \_\____/

*/
bool				ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const
{
	switch (inObject1)
	{
	case Layers::NON_MOVING:
		return inObject2 == Layers::MOVING;
	case Layers::MOVING:
		return true;
	default:
		JPH_ASSERT(false);
		return false;
	}
}


bool		ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const
{
	switch (inLayer1)
	{
	case Layers::NON_MOVING:
		return inLayer2 == BroadPhaseLayers::MOVING;
	case Layers::MOVING:
		return true;
	default:
		JPH_ASSERT(false);
		return false;
	}
}

