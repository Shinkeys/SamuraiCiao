#include "../../headers/editor/editorDebug.h"

void EditorDebug::PassCollisionDependency(CollisionDependency* dependencies)
{
    if(dependencies == nullptr)
    {
        std::cout << "Cannot inject collision. Collision dependencies is null\n";
        return;
    }
    
    _collisionDependency = dependencies;
}

void EditorDebug::RequestLineDebugUpdate(const LineDebug& newData)
{
    switch(newData.endType)
    {
    case LineDebug::EndType::END_TYPE_POINT:
        _clickRay.direction = newData.direction;
        break;

    case LineDebug::EndType::END_TYPE_DIRECTION:
        // Multiplier because direction is not a point, so would draw line of direction
        _clickRay.direction    = newData.direction * 1000.0f;
        break;
    
    default: 
        std::cout << "Undefined type of line direction to debug\n";
        break;
    }
    _clickRay.origin    = newData.origin;
    _clickRay.color     = newData.color;
}

void EditorDebug::DebugScene() const
{
    // if(_collisionDependency == nullptr)
    // {
    //     std::cout << "Can't debug in editor mode, collision debug is null\n";
    //     return;
    // }
    // _collisionDependency.collisionDebug->DrawLine(_clickRay.origin, _clickRay.direction, _clickRay.color); 
}