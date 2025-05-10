#pragma once

#include <DirectXMath.h>
#include <d3d12.h>
#include <map>

#include "Common.h"
#include "GpuResource.h"
#include "Texture.h"

struct aiMesh;
struct aiMaterial;

using namespace DirectX;
namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		static constexpr size_t indiceNumPerFace = 3;
		static_assert(indiceNumPerFace == 3, "The number of indice per face should remain 3 (Triangle)");

		using IndexType = uint32_t;
		static constexpr size_t indexSize = sizeof(IndexType);
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

			Mesh(const aiMesh& assimpMesh, const aiMaterial& material) noexcept;

			// TODO: deprecate this
			Mesh(const aiMesh& assimpMesh, const GraphicsDevice& graphicsDevice) noexcept;

			inline vector<Vertex>& GetVerticeDataOwner() noexcept
			{
				return mVertices;
			}

			inline vector<IndexType>& GetIndexDataOwner() noexcept
			{
				return mIndices;
			}

			inline ID3D12Resource* GetVertexBufferResource() noexcept
			{
				return mVertexBufferResource->GetD3d12Resource();
			}

			inline ID3D12Resource* GetIndexBufferResource() noexcept
			{
				return mIndexBufferResource->GetD3d12Resource();
			}

			inline D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() noexcept
			{
				return mVertexBufferView;
			}

			inline D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() noexcept
			{
				return mIndexBufferView;
			}

			inline const size_t GetVerticeNum() const noexcept
			{
				return mVerticesNum;
			}

			inline const size_t GetIndicesNum() const noexcept
			{
				return mIndicesNum;
			}

			std::string& GetName()
			{
				return mName;
			}

			std::string& GetTextureName(Texture::TextureType type);

		private:
			std::string mName = "";
			size_t mVerticesNum = 0;
			size_t mIndicesNum = 0;
			vector<Vertex> mVertices = {};
			vector<IndexType> mIndices = {};

			std::map<Texture::TextureType, std::string> mTextureNameMap = {};

			// TODO: deprecate theses
			unique_ptr<GpuResource> mVertexBufferResource = nullptr;
			unique_ptr<GpuResource> mIndexBufferResource = nullptr;
			D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
			D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
			
			ComPtr<ID3D12Resource> mVertexBuffer;
			ComPtr<ID3D12Resource> mIndexBuffer;
		};
	}
}

