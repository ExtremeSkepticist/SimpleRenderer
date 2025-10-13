#pragma once

#include "Material.h"

namespace sr
{
	namespace rendering
	{
		Material::Material(const string_view albedoTextureName) noexcept
			: mAlbedoTexture(albedoTextureName.data())
		{
		}

		Material::~Material() = default;
	}
}