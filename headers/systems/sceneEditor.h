#include "../types/types.h"
#include "../../headers/assets/assetManager.h"

class SceneEditor
{
private:
    glm::vec4 _rayCoords;
    uint32_t _fbo{0};
    uint32_t _depthTex{0};
    uint32_t _colorTex{0};
    std::pair<uint32_t, uint32_t> _viewportExtent{0, 0};
    void DrawScene(AssetManager& manager);
public:
    void SelectObject(const Window& window, const Matrices& matrices);
    void Prepare(uint32_t width, uint32_t height);
};