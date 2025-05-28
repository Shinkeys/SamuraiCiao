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
    SamuraiInterface::InitImgui(_window);
    _renderInstance.Initialize(_width, _height);

    Shader mainShader;
    mainShader.LoadShaders("model.vert", "model.frag");
    mainShader.UseShader();

    ObjectDescriptor characterObject;
    characterObject.name     = "character";
    characterObject.fileName = "character.obj";
    characterObject.storageFolder = "Character";
    characterObject.type = EntityType::TYPE_COMPOUND_STATIC_MESH;
    _assetManager.AddEntityToLoad(characterObject);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    _assetManager.ApplyTransformation(characterObject.name, model);

    ObjectDescriptor sponza;
    sponza.name     = "sponza";
    sponza.fileName = "sponza.obj";
    sponza.storageFolder = "Sponza";
    // Temporary as doesn't need collider for now
    sponza.type = EntityType::TYPE_COMPOUND_DYNAMIC_MESH;
    _assetManager.AddEntityToLoad(sponza);
    glm::mat4 sponzaModel = glm::mat4(1.0f);
    sponzaModel = glm::translate(sponzaModel, glm::vec3(100.0f, 0.0f, 0.0f));
    sponzaModel = glm::scale(sponzaModel, glm::vec3(0.03f));
    _assetManager.ApplyTransformation(sponza.name, sponzaModel);


    ObjectDescriptor groundObject;
    groundObject.name = "ground";
    groundObject.fileName = "scene.gltf";
    groundObject.type = EntityType::TYPE_COMPOUND_STATIC_MESH;
    groundObject.storageFolder = "Ground";
    _assetManager.AddEntityToLoad(groundObject);
    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, 0.0f, 0.0f));
    groundModel = glm::scale(groundModel, glm::vec3(5   .0f));
    _assetManager.ApplyTransformation(groundObject.name, groundModel);

    Temple templeObject(_assetManager);
    templeObject.Prepare(mainShader);

    // skybox
    Skybox skyboxObject(_assetManager);
    skyboxObject.Prepare();

    // Lantern to do
    _lightSources.Prepare();
    _renderInstance.PassLightSources(_lightSources);
    
    RenderManager::DispatchMeshToDraw(characterObject, _assetManager);
    RenderManager::DispatchMeshToDraw(groundObject, _assetManager);
    RenderManager::DispatchMeshToDraw(sponza, _assetManager);
    RenderManager::AddShaderByType(std::move(mainShader), RenderPassType::RENDER_MAIN);
    
    // shadows
    _shadowsHelper.PassLanterns(&_lightSources);
    _shadowsHelper.Prepare();
    _assetManager.BindStructures();


    // // collision
    _collision.PassAssetManager(&_assetManager);
    _collision.Prepare();
    // particles
    _particles.Prepare();

    // scene editor
    _editor.PassWindow(Window::GetWindowPointer());
    _editor.PassManager(&_assetManager);
    _editor.PassCollisionDependency(_collision.GetCollisionDependency());
    _editor.Initialize();
  
    return true;
}

void Core::Update()
{
    SamuraiCameras::g_activeCamera->Update(Window::GetWindowPointer());
    _collision.Update();
    _editor.Update();
    _renderInstance.Update(Window::GetWindowPointer());

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
    
    
    _renderInstance.Render(_assetManager);
    
    
    _particles.RenderParticles();
    
    
    _collision.WorkWithCollisionDebug();

    
    // Making editor objects in front of others
    glClear(GL_DEPTH_BUFFER_BIT);
    _editor.Render();
    
    InterfaceUpdate();

    glfwSwapBuffers(Window::_window);
    glfwPollEvents();
}

void Core::InterfaceUpdate()
{
    if(Window::GetKeysState().sceneEditor)
    {
        // must be first: creating window
        SamuraiInterface::DebugWindow(Window::_width / 4, Window::_height, SamuraiCameras::g_mainCamera.GetPosition());
        
        _shadowsHelper.DebugShadows();
        _particles.EnableParticles();
        _collision.InterfaceUpdate();
        _editor.InterfaceUpdate();
        _lightSources.InterfaceLightsCreation();

        // must be last: finishing frame
        SamuraiInterface::RenderImgui();
    }
}