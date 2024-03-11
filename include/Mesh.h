#pragma once

#include <DirectXMath.h>

#include "Common.h"

using namespace DirectX;

struct aiMesh;

class Mesh
{
public:
	Mesh() = delete;
	Mesh(const aiMesh& assimpMesh);

private:
	vector<XMFLOAT3> mVertices;
	vector<XMFLOAT2> mTexCoords;
	const string mName;
};

