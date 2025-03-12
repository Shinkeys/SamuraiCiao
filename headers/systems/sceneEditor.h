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
    void DrawScene(const AssetManager& manager);
    void SelectObject(const Window& window, const Matrices& matrices);
public:
    void PrepareObjectSelection(uint32_t width, uint32_t height);
    void HandleObjectSelection(const Window& window, 
        const Matrices& matrices, const AssetManager& manager);
};