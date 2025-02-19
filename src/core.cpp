#include "../headers/core.h"
#include "../headers/systems/shaders.h"
#include "../headers/backend/openglbackend.h"
#include "../headers/systems/interface.h"
bool Core::Initialize()
{
    if(!Window::Initialize())
    {
        return false;
    }
    OpenglBackend::SetupOpenglBackendData(_width, _height);
    SamuraiInterface::InitImgui(Window::_window);

    _mainShader.LoadShaders("main.vert", "main.frag");
    _mainShader.UseShader();


    const std::string characterObjectName = "character.obj";
    _assetManager.AddEntityToLoad(characterObjectName, _mainShader);
    Matrices matrices;
    matrices.model = glm::translate(matrices.model, glm::vec3(0.0f, 5.0f, 0.0f));
    _assetManager.ApplyMVPMatrices(characterObjectName, matrices);
    
    Temple templeObject(_assetManager);
    templeObject.Prepare(_mainShader);
    _assetManager.BindStructures();


    return true;
}

void Core::Update()
{
    _camera.Update(Window::GetWindowPointer());

    _mainShader.SetMat4x4("view", _camera.GetMVP().view);
    _mainShader.SetMat4x4("projection", _camera.GetMVP().projection);
}

void Core::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    _assetManager.GlobalDraw();
    
    SamuraiInterface::DebugWindow(_camera.GetPosition());
    SamuraiInterface::RenderImgui();
    glfwSwapBuffers(Window::_window);
    glfwPollEvents();
}