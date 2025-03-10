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
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0f));
    model = glm::scale(model, glm::vec3(0.5f));
    _assetManager.ApplyTransformation(characterObjectName, model);


    const std::string groundObjectName = "ground.gltf";
    _assetManager.AddEntityToLoad(groundObjectName);
    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::scale(groundModel, glm::vec3(5.0f));
    _assetManager.ApplyTransformation(groundObjectName, groundModel);



    // testing normal mapping
    const std::string normalObjectName = "testnormalmapping.gltf";
    _assetManager.AddEntityToLoad(normalObjectName);
    glm::mat4 normalModel = glm::mat4(1.0);
    normalModel = glm::translate(normalModel, glm::vec3(0.0f, -5.0f, 40.0f));
    normalModel = glm::scale(normalModel, glm::vec3(5.5f));
    _assetManager.ApplyTransformation(normalObjectName, normalModel);



    Temple templeObject(_assetManager);
    templeObject.Prepare(mainShader);

    // skybox
    Skybox skyboxObject(_assetManager);
    skyboxObject.Prepare();

    // Lantern to do
    _lanternsObjects.Prepare(_assetManager);
    
    RenderManager::DispatchMeshToDraw(normalObjectName, _assetManager, EntityType::TYPE_MESH);
    RenderManager::DispatchMeshToDraw(characterObjectName, _assetManager, EntityType::TYPE_MESH);
    RenderManager::DispatchMeshToDraw(groundObjectName, _assetManager, EntityType::TYPE_MESH);
    RenderManager::AddShaderByType(std::move(mainShader), RenderPassType::RENDER_MAIN);
    
    // shadows
    _shadowsHelper.PassLanterns(&_lanternsObjects);
    _shadowsHelper.Prepare();
    _assetManager.BindStructures();

    _particles.Prepare();
  
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

    
    _shadowsHelper.DrawDepthScene(_assetManager, _camera);
    
    OpenglBackend::SetViewport(Window::_width, Window::_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    RenderManager::GlobalDraw(_assetManager);

    _particles.RenderParticles();
    
    if(Window::GetKeysState().showImgui)
    {
        SamuraiInterface::DebugWindow(_camera.GetPosition());
        // must be first: creating window
        _shadowsHelper.DebugShadows();
        _particles.EnableParticles();

        // must be last: finishing frame
        SamuraiInterface::RenderImgui();
    }
        
    glfwSwapBuffers(Window::_window);
    glfwPollEvents();
}