#include <assimp/scene.h>
#include <wincodec.h>

#include "Model.h"

// TODO: https://learn.microsoft.com/en-us/windows/win32/wic/-wic-decoder-howto-createusingstream
Model::Model(const aiScene& assimpScene, XMFLOAT3 worldCoords)
	: mWorldCoords(worldCoords)
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
		// - .gltf format with metallic-roughness material
		aiMaterial* material = assimpScene.mMaterials[i];
		
		// TODO: Check:
		// - Emissive factor
		// - Metallic-roughness factor
		// - AO factor
		int shadingMode = FALSE;
		material->Get(AI_MATKEY_SHADING_MODEL, shadingMode);
		assert(shadingMode == static_cast<aiShadingMode>(aiShadingMode_PBR_BRDF));
		
		aiString albedoMapName;
		material->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &albedoMapName);

		// Assimp recognizes metallic-roughness texture as metallic texture
		aiString metallicRoughnessMapName;
		material->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &metallicRoughnessMapName);

		aiString emissiveMapName;
		material->GetTexture(aiTextureType_EMISSIVE, 0, &emissiveMapName);

		aiString normalMapName;
		material->GetTexture(aiTextureType_NORMALS, 0, &normalMapName);
		normalMapName.C_Str();

		aiString aoMapName;
		material->GetTexture(aiTextureType_LIGHTMAP, 0, &aoMapName);
		aoMapName.C_Str();


		// Load WIC image
		//{
		//	IWICImagingFactory2* pWicFactory = nullptr;
		//	CoCreateInstance(CLSID_WICImagingFactory2, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pWicFactory));
		//	assert(pWicFactory != nullptr);

		//	ComPtr<IWICStream> pWicStream = nullptr;
		//	pWicFactory->CreateStream(pWicStream.GetAddressOf());
		//	
		//	pWicStream->InitializeFromMemory()
		//}
		__noop;
	}
}

void Model::TransformCoords(XMMATRIX transformMatrix)
{
	XMVECTOR vector = XMLoadFloat3(&mWorldCoords);
	
	vector = XMVector3TransformCoord(vector, transformMatrix);
}
