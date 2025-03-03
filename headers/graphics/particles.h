#include "../types/types.h"


class Particles
{
private:
    uint32_t _ssboId;
    uint32_t _numberOfParticles{512};
    VBOSetupUnskinnedVec4 _setup;
    void ExecuteCompShader();
    void FillParticlesPoints();
public:
    void GenerateParticles();
    void DrawParticles();
};