#pragma once

#include <iostream>
#include "../../headers/systems/shaders.h"
#include "../../headers/backend/openglbackend.h"
#include "../../headers/models/assetManager.h"

class Temple
{
private:
    EBOSetup eboSimpleBind;
    Model _models;

    AssetManager& _assetManager;
public:
    Temple(AssetManager& manager);
    void Prepare(Shader& shader);
};