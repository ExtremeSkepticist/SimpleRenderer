#include "AssetLoader.h"
#include "Entity.h"
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

		void AssetLoader::LoadAsset(gsl::czstring assetPath)
		{
			const aiScene* scene = mImporter->ReadFile(assetPath, aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
			assert(scene->HasMeshes() && scene->HasMaterials() && scene->HasTextures());

			mAssets.emplace_back(*scene);
		}

		size_t AssetLoader::GetAssetCount() const
		{
			return mAssets.size();
		}

		void AssetLoader::LoadEntity(Entity& entity, const size_t assetIndex) noexcept
		{
			Expects(assetIndex < mAssets.size());

			loadEntityRescursively(entity.GetRootNode(), *mAssets[assetIndex].mRootNode, mAssets[assetIndex]);
		}

		void AssetLoader::loadEntityRescursively(EntityNode& entityNode, const aiNode& assimpNode, const aiScene& scene)
		{
			const size_t childrenNum = assimpNode.mNumChildren;
			for (size_t i = 0; i < childrenNum; ++i)
			{
				EntityNode& newChildNode = entityNode.AddChild();
				loadEntityRescursively(newChildNode, *assimpNode.mChildren[i], scene);
			}

			const size_t meshNum = assimpNode.mNumMeshes;
			for (size_t i = 0; i < meshNum; ++i)
			{
				// How do I load assets into mesh?
				// Want to keep assimp only used in AssetLoader
				const size_t meshIndex = assimpNode.mMeshes[i];
				loadMesh(entityNode, *scene.mMeshes[meshIndex], scene);
			}
		}

		void AssetLoader::loadMesh(EntityNode& entityNode, const aiMesh& mesh, const aiScene& scene)
		{
			std::vector<Mesh::Vertex> vertices = {};
			std::vector<IndexType> indices = {};

			Expects(mesh.HasFaces() && mesh.HasNormals() && mesh.GetNumUVChannels() > 0);

			vertices.reserve(mesh.mNumVertices);
			vertices.reserve(mesh.mNumFaces * sr::rendering::indiceNumPerFace);

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

			entityNode.AddMesh(vertices, indices);
		}
	}
}