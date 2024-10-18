#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class Shader
		{
		public:
			Shader(gsl::not_null<gsl::cwzstring> shaderPath) noexcept;
			~Shader() = default;

			D3D12_SHADER_BYTECODE GetShaderByteCode() const noexcept;

		private:
			ComPtr<ID3DBlob> mShaderBlob = nullptr;
		};
	}
}