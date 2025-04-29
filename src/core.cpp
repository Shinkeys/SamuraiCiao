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

    Shader mainShader;
    mainShader.LoadShaders("model.vert", "model.frag");
    mainShader.UseShader();

    ObjectDescriptor characterObject;
    characterObject.name = "character.obj";
    characterObject.type = EntityType::TYPE_COMPOUND_STATIC_MESH;
    _assetManager.AddEntityToLoad(characterObject);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    _assetManager.ApplyTransformation(characterObject.name, model);

    ObjectDescriptor groundObject;
    groundObject.name = "ground.gltf";
    groundObject.type = EntityType::TYPE_BOX_MESH;
    _assetManager.AddEntityToLoad(groundObject);
    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::scale(groundModel, glm::vec3(5.0f));
    _assetManager.ApplyTransformation(groundObject.name, groundModel);



    // testing normal mapping
    ObjectDescriptor normalObject;
    normalObject.name = "testnormalmapping.gltf";
    normalObject.type = EntityType::TYPE_COMPOUND_STATIC_MESH;
    _assetManager.AddEntityToLoad(normalObject);
    glm::mat4 normalModel = glm::mat4(1.0);
    normalModel = glm::translate(normalModel, glm::vec3(0.0f, -5.0f, 40.0f));
    normalModel = glm::scale(normalModel, glm::vec3(5.5f));
    _assetManager.ApplyTransformation(normalObject.name, normalModel);



    Temple templeObject(_assetManager);
    templeObject.Prepare(mainShader);

    // skybox
    Skybox skyboxObject(_assetManager);
    skyboxObject.Prepare();

    // Lantern to do
    _lanternsObjects.Prepare(_assetManager);
    
    RenderManager::DispatchMeshToDraw(normalObject, _assetManager);
    RenderManager::DispatchMeshToDraw(characterObject, _assetManager);
    RenderManager::DispatchMeshToDraw(groundObject, _assetManager);
    RenderManager::AddShaderByType(std::move(mainShader), RenderPassType::RENDER_MAIN);
    
    // shadows
    _shadowsHelper.PassLanterns(&_lanternsObjects);
    _shadowsHelper.Prepare();
    _assetManager.BindStructures();


    // collision
    _collision.PassAssetManager(&_assetManager);
    _collision.Prepare();
    // particles
    _particles.Prepare();

    // scenee editor
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

        // must be last: finishing frame
        SamuraiInterface::RenderImgui();
    }
}