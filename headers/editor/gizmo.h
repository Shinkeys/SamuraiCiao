#pragma once
#include "../types/collisionTypes.h"


// temporary
enum class GizmoGroup
{
    GIZMO_PART_NONE,
    GIZMO_PART_X,
    GIZMO_PART_Y,
    GIZMO_PART_Z,
};

enum class GizmoObjects
{
    GIZMO_NONE,
    GIZMO_CUBE,
    GIZMO_CYLINDER_X,
    GIZMO_CYLINDER_Y,
    GIZMO_CYLINDER_Z,
    GIZMO_CONE_X,
    GIZMO_CONE_Y,
    GIZMO_CONE_Z,
};

enum class GizmoStatus
{
    GIZMO_STATUS_NONE,
    GIZMO_STATUS_MOVE,
    GIZMO_STATUS_ROTATE,
};

// To replace
struct GizmoPartHandle
{


    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;    
};

// Purpose: structure to set position and rotation of the WHOLE gizmo (including all the parts)
struct GizmoTotalTransform
{
    glm::vec3 worldPos;
};

// Purpose: structure to set position and rotation of some part of gizmo separately
struct GizmoPartTransform
{
    glm::mat4 rotation;

    ObjColor  color;
};

struct GizmoEBOSetup : public EBOSetup
{
    // Need those to check intersections with the vertices
    uint32_t cylinderStartIndex;
    uint32_t cylinderEndIndex;
    float cylinderRadius;

    uint32_t coneStartIndex;
    uint32_t coneEndIndex;
    float coneRadius;
};

class Gizmo
{
private:    
    CollisionDependency* _collisionDependency = nullptr;
    GizmoEBOSetup _setup;
    GizmoTotalTransform _gizmoObjectTransform;
    std::unordered_map<GizmoGroup, GizmoPartTransform> _gizmoPartDescriptors;

    void CreateGeometry();
    void CreateGizmoHandles();
    void Update();
    void Render();
public:
    void PassCollisionDependency(CollisionDependency* dependencies);
    const std::vector<Vertex>& GetGizmoAxisVertices() const { return _setup.vertices; } 
    void TranslateGizmoObject(glm::vec3 position);
    void HideGizmoObject();
    void ShowGizmoObject();
    bool CheckForIntersection(glm::vec3 rayOrigin, glm::vec3 rayDir);
    void RenderLoop();
    void Initialize();
};
