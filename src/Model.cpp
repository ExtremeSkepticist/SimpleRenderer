#include <assimp/scene.h>
#include <wincodec.h>

#include "GraphicsDevice.h"
#include "Model.h"

namespace sr
{
	namespace rendering
	{
		Model::Model(const aiScene& assimpScene, const XMFLOAT3 worldCoords) noexcept
			: mWorldCoords(worldCoords)
		{
			Expects(assimpScene.HasMeshes());
			Expects(assimpScene.HasMaterials());

			mMeshes.reserve(assimpScene.mNumMeshes);

			for (gsl::index i = 0; i < assimpScene.mNumMeshes; ++i)
			{
				mMeshes.emplace_back();

				// Ignoring recursive node processing as a precondition that only one damagedhelmet is used

				const aiMesh& mesh = *assimpScene.mMeshes[i];
				const aiMaterial& material = *assimpScene.mMaterials[mesh.mMaterialIndex];
				mMeshes[i] = make_unique<Mesh>(mesh, material);
			}
		}

		// TODO: deprecate this
		Model::Model(const aiScene& assimpScene, const XMFLOAT3 worldCoords, const GraphicsDevice& graphicsDevice) noexcept
			: mWorldCoords(worldCoords)
		{
			Expects(assimpScene.HasMeshes() == true);
			mMeshes.reserve(assimpScene.mNumMeshes);
			
			for (gsl::index i = 0; i < assimpScene.mNumMeshes; ++i)
			{
				mMeshes.emplace_back();

				mMeshes[i] = make_unique<Mesh>(*assimpScene.mMeshes[i], graphicsDevice);
			}
		}
	}
}