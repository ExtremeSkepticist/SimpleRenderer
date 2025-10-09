#include <assimp/mesh.h>
#include <assimp/material.h>

#include "GraphicsDevice.h"
#include "Mesh.h"

namespace sr
{
	namespace rendering
	{

		Mesh::Mesh(std::string_view name, std::vector<Vertex>& vertices, std::vector<IndexType>& indices, std::unique_ptr<Material>& material)
			: mName(name.data())
			, mVertices(std::move(vertices))
			, mIndices(std::move(indices))
			, mMaterial(std::move(material))
		{
		}

		Mesh::Mesh(const aiMesh& assimpMesh, const aiMaterial& material) noexcept
			: mName(assimpMesh.mName.C_Str())
			, mVerticesNum(assimpMesh.mNumVertices)
			, mIndicesNum(assimpMesh.mNumFaces* indiceNumPerFace)
			, mTextureNameMap()
		{
			Expects(mVerticesNum > 0);
			Expects(mIndicesNum > 0);
			Expects(assimpMesh.GetNumUVChannels() == 1 && assimpMesh.HasTextureCoords(0));
			Expects(assimpMesh.HasNormals());
			Expects(assimpMesh.HasFaces());

			mVertices.reserve(mVerticesNum);
			mIndices.reserve(mIndicesNum);
			for (gsl::index i = 0; i < mVerticesNum; ++i)
			{
				const XMFLOAT3 position(reinterpret_cast<float*>(&assimpMesh.mVertices[i]));
				const XMFLOAT2 texcoords(reinterpret_cast<float*>(&assimpMesh.mTextureCoords[0][i]));
				const XMFLOAT3 normal(reinterpret_cast<float*>(&assimpMesh.mNormals[i]));
				const XMFLOAT3 tangent(reinterpret_cast<float*>(&assimpMesh.mTangents[i]));
				const XMFLOAT3 binormal(reinterpret_cast<float*>(&assimpMesh.mBitangents[i]));

				const Vertex vertex = { position, texcoords, normal, tangent, binormal };
				mVertices.emplace_back(vertex);
			}

			for (gsl::index i = 0; i < assimpMesh.mNumFaces; ++i)
			{
				const aiFace& face = assimpMesh.mFaces[i];
				for (gsl::index j = 0; j < face.mNumIndices; ++j)
				{
					mIndices.emplace_back(face.mIndices[j]);
				}
			}

			aiShadingMode shadingMode;
			material.Get(AI_MATKEY_SHADING_MODEL, shadingMode);
			assert(shadingMode == aiShadingMode_PBR_BRDF);

			// TODO: Move this into material class
			// Albedo
			aiString textureName;
			material.GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &textureName);
			mTextureNameMap.emplace(make_pair(Texture::TextureType::Albedo, textureName.C_Str()));

			// Metallic-Roughness
			// Note: Assimp recognizes metallic-roughness texture as metallic texture
			material.GetTexture(AI_MATKEY_METALLIC_TEXTURE, &textureName);
			mTextureNameMap.emplace(make_pair(Texture::TextureType::MetalRoughness, textureName.C_Str()));

			material.GetTexture(aiTextureType_NORMALS, 0, &textureName);
			mTextureNameMap.emplace(make_pair(Texture::TextureType::Normal, textureName.C_Str()));

			material.GetTexture(aiTextureType_EMISSIVE, 0, &textureName);
			mTextureNameMap.emplace(make_pair(Texture::TextureType::Emissive, textureName.C_Str()));

			// Assimp detects AO map of damaged helmet as light map
			material.GetTexture(aiTextureType_LIGHTMAP, 0, &textureName);
			mTextureNameMap.emplace(make_pair(Texture::TextureType::AmbientOcclusion, textureName.C_Str()));

			Ensures(mVertices.size() == assimpMesh.mNumVertices);
			Ensures(mIndices.size() == mIndicesNum);
			Ensures(mTextureNameMap.size() == static_cast<size_t>(Texture::TextureType::Num));
		}

		const size_t Mesh::GetVerticeCount() const noexcept
		{
			return mVertices.size();
		}

		const vector<Mesh::Vertex>& Mesh::GetVertice() const noexcept
		{
			return mVertices;
		}

		const size_t Mesh::GetIndicesCount() const noexcept
		{
			return mIndices.size();
		}

		const vector<IndexType>& Mesh::GetIndice() const noexcept
		{
			return mIndices;
		}

		const std::string& Mesh::GetTextureName(Texture::TextureType type) const
		{
			return mTextureNameMap.find(type)->second;
		}
	}
}