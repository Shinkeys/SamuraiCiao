#pragma once
#include "../types/types.h"
#include "../physics/collisionDebug.h"

struct LineDebug
{
    JPH::Vec3 origin    = JPH::Vec3(0.0f, 0.0f, 0.0f);
    JPH::Vec3 direction = JPH::Vec3(0.0f, 0.0f, 0.0f);
    JPH::Color color    = JPH::Color::sYellow;

    enum class EndType
    {
        END_TYPE_POINT,
        END_TYPE_DIRECTION,
    };
    EndType endType;
};

class EditorDebug
{
private:
    LineDebug _clickRay;
    CollisionDependency _collisionDependency;

public:
    void RequestLineDebugUpdate(const LineDebug& newData);
    void PassCollisionDependency(const CollisionDependency& dependency);
    void DebugScene() const;
};