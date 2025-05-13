#include <iostream>
#include "window.h"
#include "scene/temple.h"
#include "scene/skybox.h"
#include "graphics/lightSources.h"
#include "graphics/shadows.h"
#include "systems/renderManager.h"
#include "systems/shaders.h"
#include "physics/collision.h"
#include "systems/camera.h"
#include "assets/assetManager.h"
#include "editor/editor.h"

#include "graphics/particles.h"

class Core : public Window
{
private:
    void Update() override;
    void Render() override;

    void InterfaceUpdate();
    LightSources _lanternsObjects;
    AssetManager _assetManager;
    ShadowsHelper _shadowsHelper;
    
    Collision _collision;
    Particles _particles;

    Editor _editor;
    ForwardPlusRender _renderInstance;
public:
    bool Initialize() override;
};