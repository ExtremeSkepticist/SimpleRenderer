#pragma once

#include "Common.h"

struct aiScene;
struct aiMesh;
struct aiNode;

namespace Assimp
{
	class Importer;
}

namespace sr
{
	namespace rendering
	{
		class Entity;
		class EntityNode;
		class Mesh;

		class AssetLoader
		{
		public:
			AssetLoader() = delete;
			AssetLoader() noexcept;

			void LoadAsset(gsl::czstring assetPath) noexcept;
			void LoadEntity(Entity& entity, const size_t assetIndex) noexcept;

			size_t GetAssetCount() const;

		private:
			void loadEntityRescursively(EntityNode& entityNode, const aiNode& assimpNode, const aiScene& scene);
			void loadMesh(EntityNode& entityNode, const aiMesh& mesh, const aiScene& scene) noexcept;

			std::unique_ptr<Assimp::Importer> mImporter = nullptr;
			std::vector<const aiScene> mAssets = {};
		};
	}
}