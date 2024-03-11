#include <assimp/mesh.h>

#include "Mesh.h"

Mesh::Mesh(const aiMesh& assimpMesh)
	: mName(assimpMesh.mName.C_Str())
{
	const size_t VERTEX_NUMBER = assimpMesh.mNumVertices;
	assert(VERTEX_NUMBER > 0);
	assert(assimpMesh.GetNumUVChannels() == 1);

	mVertices.reserve(VERTEX_NUMBER);
	mTexCoords.reserve(VERTEX_NUMBER);
	for (size_t i = 0; i < VERTEX_NUMBER; ++i)
	{
		const aiVector3D& VERTEX = assimpMesh.mVertices[i];
		mVertices.emplace_back(VERTEX.x, VERTEX.y, VERTEX.z);

		const aiVector3D& TEX_COORD = assimpMesh.mTextureCoords[0][i];
		mTexCoords.emplace_back(TEX_COORD.x, TEX_COORD.y);
	}

}