#include <assimp/mesh.h>

#include "Mesh.h"

Mesh::Mesh(const aiMesh& assimpMesh)
	: mVerticeNum(0)
	, mName(assimpMesh.mName.C_Str())
	, mVertexBuffer(nullptr)
	, mIndexBuffer(nullptr)
	, mVertexBufferView()
	, mIndexBufferView()
{
	assert(assimpMesh.mNumVertices > 0);
	assert(assimpMesh.GetNumUVChannels() == 1 && assimpMesh.HasTextureCoords(0) == true);
	assert(assimpMesh.HasNormals() == true);
	assert(assimpMesh.HasFaces() == true);
	
	mVerticeNum = assimpMesh.mNumVertices;
	mVertices.reserve(mVerticeNum);
	for (size_t i = 0; i < assimpMesh.mNumVertices; ++i)
	{
		XMFLOAT3 position(reinterpret_cast<float*>(&assimpMesh.mVertices[i]));
		const XMFLOAT2 TEXCOORDS(reinterpret_cast<float*>(&assimpMesh.mTextureCoords[0][i]));
		const XMFLOAT3 NORMAL(reinterpret_cast<float*>(&assimpMesh.mNormals[i]));
		
		// TODO: transform in shader
		XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&position), XMMatrixScaling(0.5, 0.5, 0.5)));
		XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&position), XMMatrixRotationX(20)));
		XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&position), XMMatrixRotationY(5)));
		XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&position), XMMatrixTranslation(0, 0, 1)));

		const Vertex VERTEX = { position, TEXCOORDS, NORMAL };
		mVertices.emplace_back(VERTEX);
	}

	assert(assimpMesh.HasFaces() == true);
	const size_t FACE_INDEX_COUNT = 3;
	mIndexNum = assimpMesh.mNumFaces * FACE_INDEX_COUNT;
	mIndices.reserve(assimpMesh.mNumFaces);
	for (size_t i = 0; i < assimpMesh.mNumFaces; ++i)
	{
		const aiFace& FACE = assimpMesh.mFaces[i];
		for (size_t j = 0; j < FACE.mNumIndices; ++j)
		{
			mIndices.emplace_back(FACE.mIndices[j]);
		}
	}
}