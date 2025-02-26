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

    Shader mainShader;
    mainShader.LoadShaders("model.vert", "model.frag");
    mainShader.UseShader();

    const std::string characterObjectName = "character.obj";
    _assetManager.AddEntityToLoad(characterObjectName);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    _assetManager.ApplyTransformation(characterObjectName, model);


    const std::string groundObjectName = "ground.gltf";
    _assetManager.AddEntityToLoad(groundObjectName);


    Temple templeObject(_assetManager);
    templeObject.Prepare(mainShader);

    // skybox
    Skybox skyboxObject(_assetManager);
    skyboxObject.Prepare();

    // Lantern
    _lanternsObjects.Prepare(_assetManager);
    
    // shadows
    _shadowsHelper.Prepare();

    RenderManager::DispatchMeshToDraw(characterObjectName, _assetManager, EntityType::TYPE_MESH);
    RenderManager::DispatchMeshToDraw(groundObjectName, _assetManager, EntityType::TYPE_MESH);
    RenderManager::AddShaderByType(std::move(mainShader), RenderPassType::RENDER_MAIN);
    
    _assetManager.BindStructures();
  
    return true;
}

void Core::Update()
{
    _camera.Update(Window::GetWindowPointer());

    // passing light pos in view to the shader
    // const glm::vec3 lightPosView = _lanternsObjects.LightPositionViewSpace(_camera.GetMVP());
    // _mainShader.SetVec3("vsInput.viewlightPos", lightPosView);
}

void Core::Render()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    
    _shadowsHelper.DrawDepthScene(_assetManager);
    
    OpenglBackend::SetViewport(Window::_width, Window::_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    RenderManager::GlobalDraw(_assetManager);
    
    if(Window::GetKeysState().showImgui)
    {
        // must be first: creating window
        SamuraiInterface::DebugWindow(_camera.GetPosition());
        _shadowsHelper.DebugShadows();
        
        // must be last: finishing frame
        SamuraiInterface::RenderImgui();
    }
        
    glfwSwapBuffers(Window::_window);
    glfwPollEvents();
}