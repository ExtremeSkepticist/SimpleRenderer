#pragma once

#include <unordered_map>

#include "Common.h"

struct aiScene;

namespace sr
{
	namespace rendering
	{
		class Texture;

		class TextureGroup
		{
		public:
			TextureGroup() noexcept;

			void AddMesh(gsl::not_null<gsl::czstring> meshName) noexcept;
			void LoadAllTexturesFromMesh(gsl::not_null<gsl::czstring> meshName);

		private:
			std::unordered_map<std::string, Texture> mTextureMap; // Key = mesh name, Value = texture
		};
	}
}