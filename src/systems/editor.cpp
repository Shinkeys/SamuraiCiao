#include "../../headers/systems/editor.h"

Editor::Editor()
{
    SamuraiCameras::g_editorCamera.SetPosition(glm::vec3(0.0f, 10.0f, 50.0f));
}

void Editor::PassWindow(Window* wnd)
{
    if(wnd == nullptr)
    {
        std::cout << "Window instance is empty. Can't use editor\n";
        return;
    }

    _window = wnd;
}

void Editor::ChangeActiveCamera()
{
    // To rethink later
	if(_window->GetKeysState().sceneEditor && !_inEditingMode)
	{
        _inEditingMode = true;
		SamuraiCameras::g_activeCamera = &SamuraiCameras::g_editorCamera;
	}
	else if(!_window->GetKeysState().sceneEditor && _inEditingMode)
    {
        _inEditingMode = false;
		SamuraiCameras::g_activeCamera = &SamuraiCameras::g_mainCamera;
    }
}

void Editor::Update()
{
    ChangeActiveCamera();

    if(_inEditingMode)
    {

    }
}