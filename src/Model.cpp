#include <assimp/scene.h>

#include "Model.h"

Model::Model(const aiScene& assimpScene)
{
	assert(assimpScene.HasMeshes() == true);
	mMeshes.reserve(assimpScene.mNumMeshes);

	for (size_t i = 0; i < assimpScene.mNumMeshes; ++i)
	{
		mMeshes.emplace_back();
		mMeshes[i] = make_unique<Mesh>(*assimpScene.mMeshes[i]);
	}

	// Material: lighting properties of a material's surface (e.g. Wood material - base color(brown), roughness, reflection, etc.)
	// Texture: Material implemented in image format
	for (size_t i = 0; i < assimpScene.mNumMaterials; ++i)
	{
		aiMaterial* material = assimpScene.mMaterials[i];
		
		aiReturn result;

		int shadingMode = 0;
		result = material->Get(AI_MATKEY_SHADING_MODEL, shadingMode);
		assert(shadingMode == static_cast<aiShadingMode>(aiShadingMode_PBR_BRDF));

		// TODO: Check for metalness, emisiveness property and get texture name
		aiString albedoName;
		result = material->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &albedoName);
		
		albedoName.C_Str();
	}
}
