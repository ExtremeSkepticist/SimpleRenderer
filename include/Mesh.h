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
	const string mName;
};

