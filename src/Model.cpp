#include <assimp/scene.h>

#include "Model.h"

Model::Model(const aiScene& assimpScene)
{
	assert(assimpScene.HasMeshes() == true);
	mMeshes.reserve(assimpScene.mNumMeshes);

	for (size_t i = 0; i < assimpScene.mNumMeshes; ++i)
	{
		mMeshes[i] = make_unique<Mesh>(*assimpScene.mMeshes[i]);
	}
}
