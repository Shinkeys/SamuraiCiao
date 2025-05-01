#pragma once
#include "../types/types.h"
#include "../../headers/assets/assetManager.h"
#include "../../headers/types/collisionTypes.h"
#include "../../headers/physics/collisionDebug.h"
#include "gizmo.h"
#include "editorDebug.h"


struct CurrentSelectionDesc
{
    std::string selectedObjectName;
    bool objectSelected;
};


// Purpose: class to work with scene editing bounded to Opengl
// Example: object selection by mouse
class SceneEditor
{
private:
    Window* _window                           = nullptr;
    EditorDebug* _editorDebug                 = nullptr;
    CollisionDependency* _collisionDependency = nullptr;
    AssetManager* _manager                    = nullptr;

    glm::vec4 _rayCoords;
    uint32_t _fbo      = 0;

    std::unordered_map<JPH::BodyID, JPH::AABox> _collidersAABBs;

    std::pair<uint32_t, uint32_t> _viewportExtent {0, 0};
    void SelectObject();


    CurrentSelectionDesc _selectedObjectDesc;
    // Work with gizmo
    Gizmo _gizmo;
    void ChangeSelectionState();
public:
    void PassAssetManager(AssetManager* manager);
    void PassEditorDebug(EditorDebug& debug);
    void PassWindow(Window* wnd);
    void PassCollisionDependency(CollisionDependency* dependencies);
    void PrepareObjectSelection();
    void HandleObjectSelection();
};