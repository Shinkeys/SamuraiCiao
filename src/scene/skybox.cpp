#include "../../headers/scene/skybox.h"

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
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    for(uint32_t i = 0; i < entityNames.size(); ++i)
    {
        const std::string loadPath = "objects/skybox/" + entityNames[i];
        unsigned char* pixels = stbi_load(loadPath.c_str(), &width, &height, &texChannels, 0);

        if(pixels)
        {
            int32_t format = texChannels == 4 ? GL_RGBA : GL_RGB;

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 
                format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
        }
        else
	    {
		    std::cerr << "Unable to load texture by path: " << loadPath << "\n";
		    return 0;
	    }

        stbi_image_free(pixels);
    }
    // setting wrap modes and mipmaps
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 


    return textureID;
}


void Skybox::Prepare()
{   
    const std::string skyboxName = "skybox.gltf";

    _skyboxShader.LoadShaders("skybox.vert", "skybox.frag");
    _assetManager.AddEntityToLoad(skyboxName, _skyboxShader);
}