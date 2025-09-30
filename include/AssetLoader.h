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
		class AssetLoader
		{
		public:
			AssetLoader() = delete;
			AssetLoader(gsl::czstring assetPath) noexcept;
		private:
			std::unique_ptr<Assimp::Importer> mImporter = nullptr;
			std::unique_ptr<const aiScene> mScene = nullptr;
		};
	}
}