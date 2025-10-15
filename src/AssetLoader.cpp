#include "AssetLoader.h"
#include "Entity.h"
#include "Material.h"
#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace sr
{
	namespace rendering
	{
		AssetLoader::AssetLoader() noexcept
			: mImporter(make_unique<Assimp::Importer>())
			, mAssets({})
		{
		}

		void AssetLoader::LoadAsset(gsl::czstring assetPath) noexcept
		{
			const aiScene* scene = mImporter->ReadFile(assetPath, aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
			assert(scene->HasMeshes() && scene->HasMaterials());

			mAssets.emplace_back(scene);
		}

		size_t AssetLoader::GetAssetCount() const
		{
			return mAssets.size();
		}

		void AssetLoader::LoadEntity(Entity& entity, const size_t assetIndex) noexcept
		{
			Expects(assetIndex < mAssets.size());
			Expects(entity.IsRenderable());

			loadEntityRescursively(reinterpret_cast<RenderableNode&>(entity.GetRootNode()), *mAssets[assetIndex]->mRootNode, *mAssets[assetIndex]);
		}

		void AssetLoader::loadEntityRescursively(RenderableNode& renderableNode, const aiNode& assimpNode, const aiScene& scene)
		{
			for (size_t i = 0; i < assimpNode.mNumChildren; ++i)
			{
				RenderableNode& newChildNode = renderableNode.AddChild();
				loadEntityRescursively(newChildNode, *assimpNode.mChildren[i], scene);
			}

			for (size_t i = 0; i < assimpNode.mNumMeshes; ++i)
			{
				// How do I load assets into mesh?
				// Want to keep assimp only used in AssetLoader
				const size_t meshIndex = assimpNode.mMeshes[i];
				loadMesh(renderableNode, *scene.mMeshes[meshIndex], scene);
			}
		}

		void AssetLoader::loadMesh(RenderableNode& renderableNode, const aiMesh& mesh, const aiScene& scene) noexcept
		{
			std::vector<Mesh::Vertex> vertices = {};
			std::vector<IndexType> indices = {};

			Expects(mesh.HasFaces() && mesh.HasNormals() && mesh.GetNumUVChannels() > 0);

			vertices.reserve(mesh.mNumVertices);
			indices.reserve(mesh.mNumFaces * sr::rendering::indiceNumPerFace);

			for (size_t i = 0; i < mesh.mNumVertices; ++i)
			{
				const XMFLOAT3 position(reinterpret_cast<float*>(&mesh.mVertices[i]));
				const XMFLOAT2 texcoords(reinterpret_cast<float*>(&mesh.mTextureCoords[0][i])); // Increase the number of channel in Vertex struct if needed
				const XMFLOAT3 normal(reinterpret_cast<float*>(&mesh.mNormals[i]));
				const XMFLOAT3 tangent(reinterpret_cast<float*>(&mesh.mTangents[i]));
				const XMFLOAT3 binormal(reinterpret_cast<float*>(&mesh.mBitangents[i]));

				const Mesh::Vertex vertex = { position, texcoords, normal, tangent, binormal };
				vertices.emplace_back(vertex);
			}

			for (gsl::index i = 0; i < mesh.mNumFaces; ++i)
			{
				const aiFace& face = mesh.mFaces[i];
				for (gsl::index j = 0; j < face.mNumIndices; ++j)
				{
					indices.emplace_back(face.mIndices[j]);
				}
			}

			// TODO: Change how mesh is initialized
			// I don't like how a lot of parameters are passed to add a mesh
			// While keeping assimp in asset loader, the number of parameters should be reduced
			// Maybe have createMesh() and pass the return value into AddMesh() or use std::move
			auto material = getMaterial(*scene.mMaterials[mesh.mMaterialIndex]);
			renderableNode.AddMesh(mesh.mName.C_Str(), vertices, indices, material);
		}

		// TODO: Load more materials than albedo
		std::shared_ptr<Material> AssetLoader::getMaterial(const aiMaterial& material) noexcept
		{
			// TODO: Add shading mode check
			// aiShadingMode shadingMode;
			// material.Get(AI_MATKEY_SHADING_MODEL, shadingMode);
			// assert(shadingMode == aiShadingMode_PBR_BRDF);

			aiString textureName;
			material.GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &textureName);

			return std::make_shared<Material>(textureName.C_Str());
		}
	}
}