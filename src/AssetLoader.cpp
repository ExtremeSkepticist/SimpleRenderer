#include "AssetLoader.h"
#include "Entity.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace sr
{
	namespace rendering
	{
		AssetLoader::AssetLoader() noexcept
			: mImporter(make_unique<Assimp::Importer>())
			, mAssets({})
		{
		}

		void AssetLoader::LoadAsset(gsl::czstring assetPath)
		{
			const aiScene* scene = mImporter->ReadFile(assetPath, aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

			mAssets.emplace_back(*scene);
		}

		size_t AssetLoader::GetAssetCount() const
		{
			return mAssets.size();
		}

		void AssetLoader::LoadEntity(Entity& entity, const size_t assetIndex) noexcept
		{
			assert(assetIndex < mAssets.size());
			loadEntityRescursively(entity.GetRootNode(), mAssets[assetIndex]);
		}

		void AssetLoader::loadEntityRescursively(EntityNode& entity, const aiScene& scene)
		{

		}

	}
}