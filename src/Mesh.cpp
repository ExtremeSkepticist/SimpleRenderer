#include <assimp/mesh.h>

#include "Mesh.h"

Mesh::Mesh(const aiMesh& assimpMesh)
	: mName(assimpMesh.mName.C_Str())
{
	assert(assimpMesh.mNumVertices > 0);
	mVertices.reserve(assimpMesh.mNumVertices);

	for (size_t i = 0; i < assimpMesh.mNumVertices; ++i)
	{
		const aiVector3D& vertex = assimpMesh.mVertices[i];
		mVertices.emplace_back(vertex.x, vertex.y, vertex.z);
	}
}