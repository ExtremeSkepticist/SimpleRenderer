#pragma once

#include <d3d12.h>

#include "Common.h"
#include "Mesh.h"

struct aiScene;

class Model
{
public:
	Model() = delete;
	Model(const aiScene& assimpScene, XMFLOAT3 worldCoords);

	inline vector<unique_ptr<Mesh>>& GetMeshes()
	{
		return mMeshes;
	}

	void TransformCoords(XMMATRIX transformMatrix);

private:
	vector<unique_ptr<Mesh>> mMeshes;
	XMFLOAT3 mWorldCoords;
};

