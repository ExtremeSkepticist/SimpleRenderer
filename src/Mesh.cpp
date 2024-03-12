#include <assimp/mesh.h>

#include "Mesh.h"

Mesh::Mesh(const aiMesh& assimpMesh)
	: mName(assimpMesh.mName.C_Str())
{
	assert(assimpMesh.mNumVertices > 0);
	assert(assimpMesh.GetNumUVChannels() == 1 && assimpMesh.HasTextureCoords(0) == true);
	assert(assimpMesh.HasNormals() == true);
	assert(assimpMesh.HasFaces() == true);
	
	mVertices.reserve(assimpMesh.mNumVertices);
	mTexCoords.reserve(assimpMesh.mNumVertices);
	mNormals.reserve(assimpMesh.mNumVertices);
	for (size_t i = 0; i < assimpMesh.mNumVertices; ++i)
	{
		const aiVector3D& VERTEX = assimpMesh.mVertices[i];
		mVertices.emplace_back(VERTEX.x, VERTEX.y, VERTEX.z);

		const aiVector3D& TEX_COORD = assimpMesh.mTextureCoords[0][i];
		mTexCoords.emplace_back(TEX_COORD.x, TEX_COORD.y);

		const aiVector3D& NORMAL = assimpMesh.mNormals[i];
		mNormals.emplace_back(NORMAL.x, NORMAL.y, NORMAL.z);
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