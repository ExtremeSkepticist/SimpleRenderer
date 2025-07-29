#pragma once

#include <unordered_map>

#include "Common.h"
#include "Texture.h"

struct aiScene;

namespace sr
{
	namespace rendering
	{
		class Mesh;
		class RenderingPipeline;

		class TextureGroup
		{
		public:
			TextureGroup() noexcept;

			void AddMesh(Mesh& mesh) noexcept;
			void LoadAllTexturesFromMesh(RenderingPipeline& pipeline) noexcept;
			Texture& GetTexture(const Mesh* mesh, Texture::TextureType type) noexcept;
		private:
			void loadTexture(const Mesh& mesh, Texture::TextureType type, GraphicsDevice& device);
			void copyTextureIntoGpu(Texture& texture, RenderingPipeline& pipeline);

			std::unordered_map<const Mesh*, std::vector<Texture>> mTextureMap; // Key = mesh address, Value = texture
		};
	}
}