#include <assimp/mesh.h>

#include "Mesh.h"

Mesh::Mesh(const aiMesh& assimpMesh)
	: mVerticeNum(0)
	, mName(assimpMesh.mName.C_Str())
	, mVertexBuffer(nullptr)
	, mVertexBufferView()
{
	assert(assimpMesh.mNumVertices > 0);
	assert(assimpMesh.GetNumUVChannels() == 1 && assimpMesh.HasTextureCoords(0) == true);
	assert(assimpMesh.HasNormals() == true);
	assert(assimpMesh.HasFaces() == true);
	
	mVerticeNum = assimpMesh.mNumVertices;
	mVertices.reserve(mVerticeNum);
	for (size_t i = 0; i < assimpMesh.mNumVertices; ++i)
	{
		const XMFLOAT3 POSITION(reinterpret_cast<float*>(&assimpMesh.mVertices[i]));
		const XMFLOAT2 TEXCOORDS(reinterpret_cast<float*>(&assimpMesh.mTextureCoords[0][i]));
		const XMFLOAT3 NORMAL(reinterpret_cast<float*>(&assimpMesh.mNormals[i]));
		
		const Vertex VERTEX = { POSITION, TEXCOORDS, NORMAL };
		mVertices.emplace_back(VERTEX);
	}

	assert(assimpMesh.HasFaces() == true);
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