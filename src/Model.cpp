#include <assimp/scene.h>

#include "Model.h"

// TODO: https://learn.microsoft.com/en-us/windows/win32/wic/-wic-decoder-howto-createusingstream
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
		// Precondition: assume a model supports: 
		// - PBR
		// - Metallic/Roughness Workflow
		aiMaterial* material = assimpScene.mMaterials[i];


		// TODO: Check:
		// - Emissive map texture & factor
		// - Metallic & roughness factor
		// - Normal map texture
		// - AO map texture name & factor
		int shadingMode = FALSE;
		material->Get(AI_MATKEY_SHADING_MODEL, shadingMode);
		assert(shadingMode == static_cast<aiShadingMode>(aiShadingMode_PBR_BRDF));
		
		aiString albedoMapName;
		material->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &albedoMapName);

		aiString metallicRoughnessMapName;
		material->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &metallicRoughnessMapName);

		//aiString emissiveMapName;
		//material->GetTexture(AI_MATKEY_TEXTURE_EMISSIVE(0), &emissiveMapName)

		//emissiveMapName.C_Str();
		//assert(a)
		__noop;
	}
}
