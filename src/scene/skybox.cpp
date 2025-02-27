#include "../../headers/scene/skybox.h"
#include "../../headers/systems/renderManager.h"
#include <stb_image.h>

Skybox::Skybox(AssetManager& manager) : _assetManager{manager}
{

}

uint32_t Skybox::StbiLoadCubeTexture(std::vector<std::string> entityNames)
{
    if(entityNames.size() == 0)
    {
        std::cout << "Cannot load skybox, vector is empty\n";
        return 0;
    }

    int32_t width;
    int32_t height;
    int32_t texChannels;

    uint32_t textureID = 0;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glTextureStorage2D(textureID, 1, GL_RGBA8, width, height);
    for(uint32_t i = 0; i < entityNames.size(); ++i)
    {
        const std::string loadPath = "objects/skybox/" + entityNames[i];
        unsigned char* pixels = stbi_load(loadPath.c_str(), &width, &height, &texChannels, 0);

        if(pixels)
        {
            int32_t format = texChannels == 4 ? GL_RGBA : GL_RGB;
            glTextureSubImage3D(textureID, 0, 0, 0, 0, width, height, format, 1, GL_UNSIGNED_BYTE, pixels);
        }
        else
	    {
		    std::cerr << "Unable to load texture by path: " << loadPath << "\n";
		    return 0;
	    }

        stbi_image_free(pixels);
    }
    // setting wrap modes and mipmaps
    glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(textureID, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 


    return textureID;
}


void Skybox::Prepare()
{   
    const std::string skyboxName = "skybox.gltf";

    Shader skyboxShader;
    skyboxShader.LoadShaders("skybox.vert", "skybox.frag");
    _assetManager.AddEntityToLoad(skyboxName);
    glm::mat4 model = glm::mat4(1.0f);;
    model = glm::scale(model, glm::vec3(0.1f));
    _assetManager.ApplyTransformation(skyboxName, model);

    
    RenderManager::DispatchMeshToDraw(skyboxName, _assetManager, EntityType::TYPE_SKYBOX);
    RenderManager::AddShaderByType(std::move(skyboxShader), RenderPassType::RENDER_SKYBOX);
}