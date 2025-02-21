#include "../types/types.h"
#include "../systems/shaders.h"
#include "../assets/assetManager.h"

class Lanterns
{
private:
    uint32_t _lightSourcesCount = 0;
    glm::vec3 _lightSourcePosition;
public:
    void Prepare(AssetManager& manager, Shader& shader);
    glm::vec3 LightPositionViewSpace(const Matrices& matrices);
};