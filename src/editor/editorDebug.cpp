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
    _collisionDependency->UpdateLineData(newData);
}

void EditorDebug::DebugScene()
{
    _collisionDependency->AddCommand(CollisionCmdList::COLLISION_DRAW_LINE);
}