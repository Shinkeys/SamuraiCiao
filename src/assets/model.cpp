#include "../../headers/assets/model.h"
#include "../../headers/backend/openglbackend.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

CurrentModelDesc Model::LoadModel(const std::filesystem::path& modelName)
{
    Assimp::Importer importer;

	std::filesystem::path pathToModel = std::filesystem::absolute("objects/models" / modelName);

	const aiScene* scene = importer.ReadFile(pathToModel.string(), aiProcess_Triangulate | 
    aiProcess_FlipUVs | aiProcess_CalcTangentSpace |  aiProcess_PreTransformVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Assimp: unable to load your model\n" << importer.GetErrorString() << "\n";
		return {}; // to do
	}

	// to pass to asset manager
	CurrentModelDesc modelDescriptor;

	ProcessNode(scene->mRootNode, scene, modelDescriptor);
	modelDescriptor.modelName = modelName.string();

	return modelDescriptor;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene, CurrentModelDesc& modelDescriptor)
{
    for (uint32_t i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, modelDescriptor);
	}

	for (uint32_t i = 0; i < node->mNumChildren; ++i)
	{
		ProcessNode(node->mChildren[i], scene, modelDescriptor);
	}
}

void Model::ProcessMesh(aiMesh* aiMesh, const aiScene* scene, CurrentModelDesc& modelDescriptor)
{
    _allModelsData.vertices.reserve(aiMesh->mNumVertices);

    std::vector<Vertex> sortedVertices;
	sortedVertices.reserve(aiMesh->mNumVertices);

    for (uint32_t i = 0; i < aiMesh->mNumVertices; ++i)
	{
		Vertex vertices;
	
		vertices.position.x = aiMesh->mVertices[i].x;
		vertices.position.y = aiMesh->mVertices[i].y;
		vertices.position.z = aiMesh->mVertices[i].z;

		if (aiMesh->mTextureCoords[0])
		{
			vertices.texCoords.x = aiMesh->mTextureCoords[0][i].x;
			vertices.texCoords.y = aiMesh->mTextureCoords[0][i].y;
		}
		else { vertices.texCoords = glm::vec2(0.0f); }

		if (aiMesh->HasNormals())
		{
			vertices.normal.x = aiMesh->mNormals[i].x;
			vertices.normal.y = aiMesh->mNormals[i].y;
			vertices.normal.z = aiMesh->mNormals[i].z;
		}
		if(aiMesh->HasTangentsAndBitangents())
		{
			vertices.tangents.x = aiMesh->mTangents[i].x;
			vertices.tangents.y = aiMesh->mTangents[i].y;
			vertices.tangents.z = aiMesh->mTangents[i].z;
		}
		sortedVertices.push_back(vertices);
	}

    static uint32_t meshStartIndexInVAO = 0;
	modelDescriptor.meshIndexOffset.push_back(meshStartIndexInVAO);

	uint32_t currentMeshSize = 0;
	for (uint32_t i = 0; i < aiMesh->mNumFaces; ++i)
	{
		aiFace face = aiMesh->mFaces[i];
		for (uint32_t j = 0; j < face.mNumIndices; ++j)
		{
			_allModelsData.indices.push_back(meshStartIndexInVAO);
            // to check: std move
			_allModelsData.vertices.push_back(sortedVertices[face.mIndices[j]]);


			++meshStartIndexInVAO;

			++currentMeshSize;
		}
	}
    // storing amount of vertices of current mesh
    modelDescriptor.currMeshVertCount.push_back(currentMeshSize);

    if (aiMesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[aiMesh->mMaterialIndex];
		std::array<aiTextureType, 4> textureTypes
		{
			aiTextureType_DIFFUSE,
			aiTextureType_SPECULAR,
			aiTextureType_EMISSIVE,
			aiTextureType_NORMALS
		};

		ProcessMaterial(material, textureTypes, modelDescriptor);
	}
}

uint32_t Model::StbiLoadTexture(const char* fileName, bool gamma)
{
	int texWidth, texHeight, texChannels;

	uint32_t textureID = 0;
	unsigned char* pixels = stbi_load(fileName, &texWidth, &texHeight, &texChannels, 0);
	
	
	if(pixels)
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &textureID);

		if(texChannels > 4 || texChannels < 1)
		{
			std::cout << "Texture: " << fileName << " has incorrect amount of depth channels!";
			return 0;
		}

		int32_t format;
		glTextureStorage2D(textureID, 1, GL_RGBA8, texWidth, texHeight);
		if(gamma)
		{
			format = texChannels == 4 ? GL_SRGB_ALPHA : GL_SRGB;
			glTextureSubImage2D(textureID, 0, 0, 0, texWidth, texHeight, texChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, pixels);
		}
		else
		{
			format = texChannels == 4 ? GL_RGBA : GL_RGB;
			glTextureSubImage2D(textureID, 0, 0, 0, texWidth, texHeight, format, GL_UNSIGNED_BYTE, pixels);
		}
		// creating a mipmap to use downscaled texture if distance to the object is long
		// min_filter = downscale, mag_filter = upscale
		glGenerateTextureMipmap(textureID);

		glTextureParameteri(textureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		// setting min and mag for mip mapping
		glTextureParameteri(textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// doesnt need for mag, because mipmapping are used for downscale(not upscale)
		glTextureParameteri(textureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::cout << "Loaded texture: " << fileName << "\n";
	}
	else
	{
		std::cerr << "Unable to load texture by path: " << fileName << "\n";
		return 0;
	}

	stbi_image_free(pixels);

	return textureID;
}	

void Model::ProcessMaterial(aiMaterial* material, 
	std::array<aiTextureType, 4> textureTypes, CurrentModelDesc& modelDescriptor)
{
	const std::filesystem::path texturesFolder = "objects/";
	// going through all passed textures types to load every present texture
	// storing location of every texture if present, else would be 0
	ModelTexDesc textureDescriptor;
	for(uint32_t i = 0; i < textureTypes.size(); ++i)
	{
		const uint32_t currentTextureCount = material->GetTextureCount(textureTypes[i]);
		// going through all textures of current type for current mesh
		for(uint32_t j = 0; j < currentTextureCount; ++j)
		{
			aiString str;
			material->GetTexture(textureTypes[i], j, &str);
			
			const std::filesystem::path pathToLoadTex = std::filesystem::absolute(texturesFolder / str.C_Str()); 
			
			// need this for gamma correction. diffuse textures mostly in sRGB space, so in non linear space
			// so need to translate it to linear space to not apply gamma twice
			int32_t textureId;
			if(textureTypes[i] == aiTextureType_DIFFUSE)
			{
				textureId = StbiLoadTexture(pathToLoadTex.string().c_str(), true);
			}
			else
			{
				textureId = StbiLoadTexture(pathToLoadTex.string().c_str());
			} 

			switch (textureTypes[i])
			{
			case aiTextureType_DIFFUSE: textureDescriptor.diffuseId = textureId; break;
			case aiTextureType_SPECULAR: textureDescriptor.specularId = textureId; break;
			case aiTextureType_NORMALS: textureDescriptor.normalId = textureId; break;
			case aiTextureType_EMISSIVE: textureDescriptor.emissionId = textureId; break;
			}
		}
	}
	modelDescriptor.textureIDs.push_back(textureDescriptor);

}

