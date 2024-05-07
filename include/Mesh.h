#pragma once

#include <DirectXMath.h>
#include <d3d12.h>

#include "Common.h"

using namespace DirectX;

struct aiMesh;

class Mesh
{
public:
	typedef struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT2 TexCoords;
		XMFLOAT3 Normal;
	} Vertex;


	Mesh() = delete;
	Mesh(const aiMesh& assimpMesh);

	inline ComPtr<ID3D12Resource>& GetVertexBuffer()
	{
		return mVertexBuffer;
	}

private:
	vector<Vertex> mVertices;
	vector<size_t> mIndices;

	ComPtr<ID3D12Resource> mVertexBuffer;

	const string mName;
};

