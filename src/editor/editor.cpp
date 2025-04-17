#include "../../headers/editor/editor.h"
#include "../../headers/systems/interface.h"


Editor::Editor()
{
    SamuraiCameras::g_editorCamera.SetPosition(glm::vec3(0.0f, 10.0f, 50.0f));
    SamuraiCameras::g_editorCamera.SetSpeed(25.0f);
}

void Editor::PassManager(AssetManager* manager)
{
    if(manager == nullptr)
    {
        std::cout << "Manager instance is empty. Can't use editor\n";
        return;
    }

    _manager = manager;
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


void Editor::PassCollisionDependency(const CollisionDependency& dependencies)
{
    if(dependencies.collisionDebug != nullptr)
    {
        _collisionDependency.collisionDebug = dependencies.collisionDebug;
    }
    else std::cout << "Cannot inject collision. Collision debug is null\n";
    
    if(dependencies.physSystem != nullptr)
    {
        _collisionDependency.physSystem = dependencies.physSystem;
    }
    else std::cout << "Cannot inject collision. Physics system is null\n";
}

void Editor::Initialize()
{
    _sceneEditor.PassWindow(_window);
    _sceneEditor.PassCollisionDependency(_collisionDependency);
    _sceneEditor.PassEditorDebug(_editorDebug);
    _sceneEditor.PrepareObjectSelection();
    _editorDebug.PassCollisionDependency(_collisionDependency);

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

void Editor::InterfaceUpdate()
{
    ImGui::NewLine();
    ImGui::Text("Editor:");

    const float camSpeed = SamuraiCameras::g_editorCamera.GetSpeed();

    float potentialNewSpeed = camSpeed;
    ImGui::DragFloat("Edit cam speed", &potentialNewSpeed, 1.0f, 0.0f, 100.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

    if(camSpeed != potentialNewSpeed)
        SamuraiCameras::g_editorCamera.SetSpeed(potentialNewSpeed);
}

void Editor::Render()
{
    if(_inEditingMode)
    {
        _sceneEditor.HandleObjectSelection();
        _editorDebug.DebugScene();
    }
}

void Editor::Update()
{
    ChangeActiveCamera();
    
    if(_inEditingMode)
    {

    }

}