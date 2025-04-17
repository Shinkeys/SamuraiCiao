#include "../types/types.h"
#include "../systems/camera.h"
#include "sceneEditor.h"
#include "editorDebug.h"

enum class SceneStatus
{
    SCENE_PLAYER_DRAW,
    SCENE_EDITOR_DRAW,
};

class Editor
{
private:
    SceneEditor _sceneEditor;
    EditorDebug _editorDebug;

    bool _inEditingMode { false };
    AssetManager* _manager = nullptr;
    Window*       _window  = nullptr;
    CollisionDependency _collisionDependency;

    SceneStatus _sceneStatus;
    void ChangeActiveCamera();
public:
    void Initialize();
    void Update();
    void Render();
    void InterfaceUpdate();
    void PassWindow(Window* wnd);
    void PassCollisionDependency(const CollisionDependency& dependency);
    void PassManager(AssetManager* manager);
    Editor();
};
