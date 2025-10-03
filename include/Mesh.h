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

		class Material;

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
			Mesh(std::string_view name, std::vector<Vertex>& vertices, std::vector<IndexType>& indices, std::unique_ptr<Material>& material);


			// TO BE DEPRECATED
			Mesh(const aiMesh& assimpMesh, const aiMaterial& material) noexcept;

			inline const vector<Vertex>& GetVertice() const noexcept
			{
				return mVertices;
			}

			inline const vector<IndexType>& GetIndice() const noexcept
			{
				return mIndices;
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

			const std::string& GetTextureName(Texture::TextureType type) const;

		private:
			std::string mName = "";
			size_t mVerticesNum = 0;
			size_t mIndicesNum = 0;
			std::vector<Vertex> mVertices = {};
			std::vector<IndexType> mIndices = {};

			std::unique_ptr<Material> mMaterial = nullptr;
			std::map<Texture::TextureType, std::string> mTextureNameMap = {};
		};
	}
}

