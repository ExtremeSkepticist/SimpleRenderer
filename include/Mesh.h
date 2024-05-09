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

	inline vector<Vertex>& GetVerticeData()
	{
		return mVertices;
	}

	inline ComPtr<ID3D12Resource>& GetVertexBuffer()
	{
		return mVertexBuffer;
	}

	inline D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()
	{
		return mVertexBufferView;
	}

	inline const size_t GetVerticeNum() const
	{
		return mVerticeNum;
	}

private:
	vector<Vertex> mVertices;
	vector<size_t> mIndices;

	ComPtr<ID3D12Resource> mVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;

	size_t mVerticeNum;
	const string mName;
};

