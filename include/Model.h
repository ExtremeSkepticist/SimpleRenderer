#pragma once

#include "Common.h"
#include "Mesh.h"

struct aiScene;

class Model
{
public:
	Model() = delete;
	Model(const aiScene& assimpScene);
	 
private:
	vector<unique_ptr<Mesh>> mMeshes;
};

