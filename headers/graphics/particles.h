#include "../types/types.h"


class Particles
{
private:
    uint32_t _ssboId;
    uint32_t _numberOfParticles{1024};
    VBOSetupUnskinnedVec4 _setup;
    bool _particlesEnabled = true;
    void ExecuteCompShader();
    void RandomDistribution();
    void FillParticlesPoints();
    bool GenerateParticles();
    void DrawParticles();
public:
    void EnableParticles();
    void RenderParticles();
    void Prepare();

};