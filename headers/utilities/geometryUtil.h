#pragma once
#include "../types/types.h"


namespace SamuraiUtils
{
    std::vector<Vertex> GenerateCylinderVertices(float radius, float height, int32_t sectorCount);
    std::vector<Vertex> GenerateConeVertices(float radius, float height, int32_t segments);
    std::vector<Vertex> GenerateCubeVertices();

    std::vector<uint32_t> GenerateCylinderIndices(int32_t sectorCount);
    std::vector<uint32_t> GenerateConeIndices(int32_t segments);
    std::vector<uint32_t> GenerateCubeIndices();
};