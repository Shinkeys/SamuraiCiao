#pragma once
#include "../types/types.h"
#include "../physics/collisionDebug.h"


class EditorDebug
{
private:
    CollisionDependency* _collisionDependency = nullptr;

public:
    void RequestLineDebugUpdate(const LineDebug& newData);
    void PassCollisionDependency(CollisionDependency* dependency);
    void DebugScene();
};