#pragma once

#include <Windows.h>

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
			~Material();

			Material() = delete;
		private:
			std::string mAlbedoTexture = "";
		};
	}
}