#include <assimp/mesh.h>

#include "GraphicsDevice.h"
#include "GpuResource.h"
#include "Mesh.h"

namespace sr
{
	namespace rendering
	{
		Mesh::Mesh(const aiMesh& assimpMesh, const GraphicsDevice& graphicsDevice) noexcept
			: mName(assimpMesh.mName.C_Str())
			, mVerticesNum(assimpMesh.mNumVertices)
			, mIndicesNum(assimpMesh.mNumFaces * indiceNumPerFace)
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

			mVertices.reserve(mVerticesNum);
			mIndices.reserve(mIndicesNum);
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

			assert(mVertices.size() == assimpMesh.mNumVertices);
			assert(mIndices.size() == mIndicesNum);

			// TODO: Use DEFAULT heap instead
			// Initialize vertex buffer
			const size_t vertexDataSize = sizeof(sr::rendering::Mesh::Vertex) * mVerticesNum;
			mVertexBufferResource = make_unique<GpuResource>(L"Vertex buffer resource", vertexDataSize
				, ResourceHeapType::Upload, ResourceState::GenericRead, graphicsDevice);
			{
				uint8_t* mappedMemoryForVertexBuffer = nullptr;
				CD3DX12_RANGE readRange(0, 0);
				ComPtr<ID3D12Resource>& vertexBufferOwner = mVertexBufferResource->GetD3d12ResourceOwner();

				vertexBufferOwner->Map(0, &readRange, reinterpret_cast<void**>(&mappedMemoryForVertexBuffer));
				memcpy(mappedMemoryForVertexBuffer, &mVertices[0], vertexDataSize);
				vertexBufferOwner->Unmap(0, nullptr);

				mVertexBufferView.BufferLocation = vertexBufferOwner->GetGPUVirtualAddress();
				mVertexBufferView.StrideInBytes = sizeof(Vertex);
				mVertexBufferView.SizeInBytes= vertexDataSize;
			}

			// Initialize index buffer
			const size_t indexDataSize = mIndicesNum * indexSize;
			mIndexBufferResource = make_unique<GpuResource>(L"Index buffer resource", indexDataSize
				, ResourceHeapType::Upload, ResourceState::GenericRead, graphicsDevice);
			{
				uint8_t* mappedMemoryForIndexBuffer = nullptr;
				CD3DX12_RANGE readRange(0, 0);
				ComPtr<ID3D12Resource>& indexBufferOwner = mIndexBufferResource->GetD3d12ResourceOwner();

				indexBufferOwner->Map(0, &readRange, reinterpret_cast<void**>(&mappedMemoryForIndexBuffer));
				memcpy(mappedMemoryForIndexBuffer, &mIndices[0], indexDataSize);
				indexBufferOwner->Unmap(0, nullptr);

				mIndexBufferView.BufferLocation = indexBufferOwner->GetGPUVirtualAddress();
				mIndexBufferView.SizeInBytes = indexDataSize;
				mIndexBufferView.Format = indexFormat;
			}
		}
	}
}