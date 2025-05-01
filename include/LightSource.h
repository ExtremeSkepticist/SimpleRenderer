#pragma once

#include <DirectXMath.h>
#include <inttypes.h>

using namespace DirectX;

namespace sr
{
	namespace rendering
	{
		struct LightSource
		{
			XMFLOAT3 Color;
			uint32_t Intensity;
		};

		struct PointLight : public LightSource
		{
			XMFLOAT3 Position;
		};
	}
}
