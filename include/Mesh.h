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
		XMFLOAT3 Tangent;
		XMFLOAT3 Binormal;
	} Vertex;


	Mesh() = delete;
	Mesh(const aiMesh& assimpMesh);

	inline vector<Vertex>& GetVerticeData()
	{
		return mVertices;
	}
	
	inline vector<UINT32>& GetIndexData()
	{
		return mIndices;
	}

	inline ComPtr<ID3D12Resource>& GetVertexBuffer()
	{
		return mVertexBuffer;
	}
	
	inline ComPtr<ID3D12Resource>& GetIndexBuffer()
	{
		return mIndexBuffer;
	}

	inline D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView()
	{
		return mVertexBufferView;
	}

	inline D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView()
	{
		return mIndexBufferView;
	}

	inline const size_t GetVerticeNum() const
	{
		return mVerticeNum;
	}

	inline const size_t GetIndexNum() const
	{
		return mIndexNum;
	}

private:
	vector<Vertex> mVertices;
	vector<UINT32> mIndices;

	ComPtr<ID3D12Resource> mVertexBuffer;
	ComPtr<ID3D12Resource> mIndexBuffer;
	D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW mIndexBufferView;

	size_t mVerticeNum;
	size_t mIndexNum;
	const string mName;
};

