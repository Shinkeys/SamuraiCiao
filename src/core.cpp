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
    matrices.model = glm::translate(matrices.model, glm::vec3(0.0f, 3.0f, 0.0f));
    matrices.model = glm::scale(matrices.model, glm::vec3(0.5f));
    _assetManager.ApplyMVPMatrices(characterObjectName, matrices);

    const std::string groundObjectName = "ground.gltf";
    _assetManager.AddEntityToLoad(groundObjectName, _mainShader);
    _assetManager.ApplyMVPMatrices(groundObjectName, matrices);

    Temple templeObject(_assetManager);
    templeObject.Prepare(_mainShader);

    // skybox
    Skybox skyboxObject(_assetManager);
    skyboxObject.Prepare();

    // Lantern
    _lanternsObjects.Prepare(_assetManager, _mainShader);
    
    
    _assetManager.BindStructures();
    return true;
}

void Core::Update()
{
    _camera.Update(Window::GetWindowPointer());

    _mainShader.SetMat4x4("view", _camera.GetMVP().view);
    _mainShader.SetMat4x4("projection", _camera.GetMVP().projection);

    // passing light pos in view to the shader
    const glm::vec3 lightPosView = _lanternsObjects.LightPositionViewSpace(_camera.GetMVP());
    _mainShader.SetVec3("vsInput.viewlightPos", lightPosView);
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