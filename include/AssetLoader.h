#pragma once

#include "Common.h"

struct aiScene;
struct aiMesh;
struct aiNode;
struct aiMaterial;

namespace Assimp
{
	class Importer;
}

namespace sr
{
	namespace rendering
	{
		class Entity;
		class RenderableNode;
		class Mesh;
		class Material;

		class AssetLoader
		{
		public:
			AssetLoader() = delete;
			AssetLoader() noexcept;

			void LoadAsset(gsl::czstring assetPath) noexcept;
			void LoadEntity(Entity& entity, const size_t assetIndex) noexcept;

			size_t GetAssetCount() const;

		private:
			void loadEntityRescursively(RenderableNode& renderableNode, const aiNode& assimpNode, const aiScene& scene);
			void loadMesh(RenderableNode& renderableNode, const aiMesh& mesh, const aiScene& scene) noexcept;
			std::unique_ptr<Material> getMaterial(const aiMaterial& material) noexcept;

			std::unique_ptr<Assimp::Importer> mImporter = nullptr;
			std::vector<const aiScene> mAssets = {};
		};
	}
}