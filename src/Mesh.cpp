#include <assimp/mesh.h>

#include "GpuResource.h"
#include "Mesh.h"

namespace sr
{
	namespace rendering
	{
		Mesh::Mesh(const aiMesh& assimpMesh)
			: mName(assimpMesh.mName.C_Str())
			, mVerticesNum(assimpMesh.mNumVertices)
			, mIndicesNum(assimpMesh.mNumFaces * faceIndiceNum)
			, mVertices(mVerticesNum)
			, mIndices(mIndicesNum)
			, mVertexBufferResource(nullptr)
			, mIndexBufferResource(nullptr)
			, mVertexBufferView()
			, mIndexBufferView()
			, mVertexBuffer(nullptr)
			, mIndexBuffer(nullptr)
		{
			Expects(mVerticesNum > 0);
			Expects(mIndicesNum > 0);
			Expects(assimpMesh.GetNumUVChannels() == 1 && assimpMesh.HasTextureCoords(0) == true);
			Expects(assimpMesh.HasNormals());
			Expects(assimpMesh.HasFaces());

			for (gsl::index i = 0; i < mVerticesNum; ++i)
			{
				// TODO: might have to change this to XMFLOAT4
				const XMFLOAT3 position(reinterpret_cast<float*>(&assimpMesh.mVertices[i]));
				const XMFLOAT2 texcoords(reinterpret_cast<float*>(&assimpMesh.mTextureCoords[0][i]));
				const XMFLOAT3 normal(reinterpret_cast<float*>(&assimpMesh.mNormals[i]));
				const XMFLOAT3 tangent(reinterpret_cast<float*>(&assimpMesh.mTangents[i]));
				const XMFLOAT3 binormal(reinterpret_cast<float*>(&assimpMesh.mBitangents[i]));

				const Vertex vertex = { position, texcoords, normal, tangent, binormal};
				mVertices.emplace_back(vertex);
			}

			for (size_t i = 0; i < assimpMesh.mNumFaces; ++i)
			{
				const aiFace& face = assimpMesh.mFaces[i];
				for (size_t j = 0; j < face.mNumIndices; ++j)
				{
					mIndices.emplace_back(face.mIndices[j]);
				}
			}

			const size_t vertexBufferSize = sizeof(sr::rendering::Mesh::Vertex) * mVerticesNum;

			mVertexBufferResource = make_unique<GpuResource>(L"Vertex buffer resource", vertexBufferSize,)
			Ensures(mVertices.size() == assimpMesh.mNumVertices);
			Ensures(mIndices.size() == mIndicesNum);
		}
	}
}