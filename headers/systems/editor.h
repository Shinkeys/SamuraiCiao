#include "../types/types.h"
#include "camera.h"

enum class SceneStatus
{
    SCENE_PLAYER_DRAW,
    SCENE_EDITOR_DRAW,
};

class Editor
{
private:
    bool _inEditingMode { false };
    Window* _window = nullptr;
    SceneStatus _sceneStatus;
    void ChangeActiveCamera();
public:
    void Update();
    void PassWindow(Window* wnd);
    Editor();
};
