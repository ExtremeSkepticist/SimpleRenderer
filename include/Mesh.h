#pragma once

#include <DirectXMath.h>
#include <d3d12.h>

#include "Common.h"

struct aiMesh;

using namespace DirectX;
namespace sr
{
	namespace rendering
	{
		class GpuResource;
		class GraphicsDevice;

		static constexpr size_t indiceNumPerFace = 3;
		static_assert(indiceNumPerFace == 3, "The number of indice per face should remain 3 (Triangle)");

		static constexpr size_t indexSize = sizeof(uint32_t);
		static constexpr DXGI_FORMAT indexFormat = DXGI_FORMAT_R32_UINT;

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
			Mesh(const aiMesh& assimpMesh, const GraphicsDevice& graphicsDevice) noexcept;

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
				return mVerticesNum;
			}

			inline const size_t GetIndexNum() const
			{
				return mIndicesNum;
			}

		private:
			std::string mName = "";
			size_t mVerticesNum = 0;
			size_t mIndicesNum = 0;
			vector<Vertex> mVertices = {};
			vector<UINT32> mIndices = {};

			unique_ptr<GpuResource> mVertexBufferResource = nullptr;
			unique_ptr<GpuResource> mIndexBufferResource = nullptr;
			D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
			D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
			
			ComPtr<ID3D12Resource> mVertexBuffer;
			ComPtr<ID3D12Resource> mIndexBuffer;
		};
	}
}

