#pragma once

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		// Precondition: only expects a PBR material
		class Material
		{
		public:
			Material(const string_view albedoTextureName) noexcept;

			Material() = delete;
			~Material() = default;
		private:
			std::string mAlbedoTexture = "";
		};
	}
}