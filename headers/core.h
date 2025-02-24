#include <iostream>
#include "window.h"
#include "scene/temple.h"
#include "scene/skybox.h"
#include "scene/lanterns.h"
#include "graphics/shadows.h"
#include "systems/shaders.h"
#include "systems/camera.h"
#include "assets/assetManager.h"

class Core : public Window
{
private:
    void Update() override;
    void Render() override;
    Shader _mainShader;
    Camera _camera;
    Lanterns _lanternsObjects;
    AssetManager _assetManager;
    ShadowsHelper _shadowsHelper;
public:
    bool Initialize() override;
};