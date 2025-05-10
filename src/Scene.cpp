#include <assimp\scene.h>

#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "TextureGroup.h"

namespace sr
{
	namespace rendering
	{

		// Precondition
		// - assumes there's only one model as the program's currently using assimp
		// - assumes rendering only one DamagedHelmet, so ignoring recursive node-processing
		// TODO: Switch to scene graph when needed
		Scene::Scene(const aiScene& scene) noexcept
			: mModels()
			, mCamera(nullptr)
		{
			// TODO: recurse each mesh in Model class, initialize with Texture, name, etc.
			constexpr XMFLOAT3 modelWorldPosition(0, 4, 2); // original: (0, 4, 2);
			mModels.emplace_back(scene, modelWorldPosition);
		}
	}
}