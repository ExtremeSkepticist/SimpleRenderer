#include "AssetLoader.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace sr
{
	namespace rendering
	{
		AssetLoader::AssetLoader(gsl::czstring assetPath) noexcept
			: mImporter(make_unique<Assimp::Importer>())
			, mScene(mImporter->ReadFile(assetPath, aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace))
		{
		}
	}
}