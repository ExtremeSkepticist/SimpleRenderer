#pragma once

#include "Common.h"

struct aiScene;
namespace Assimp
{
	class Importer;
}

namespace sr
{
	namespace rendering
	{
		class Entity;

		class AssetLoader
		{
		public:
			AssetLoader() = delete;
			AssetLoader() noexcept;

			void LoadAsset(gsl::czstring assetPath) noexcept;
			void LoadEntity(Entity& entity, const size_t assetIndex) const noexcept;

			size_t GetAssetCount() const;

		private:

			void loadEntityRescursively();
			std::unique_ptr<Assimp::Importer> mImporter = nullptr;
			std::vector<const aiScene> mAssets = {};
		};
	}
}