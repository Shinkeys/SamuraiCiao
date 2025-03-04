#pragma once
#include <iostream>
#include <random>

namespace Random
{
    std::mt19937 mt{std::random_device{}()};

    inline float RandomFloat(float min = -1.0f, float max = 1.0f)
    {
        std::uniform_real_distribution dist{min, max};

        return dist(mt);
    }
};