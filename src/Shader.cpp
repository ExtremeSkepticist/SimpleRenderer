#include <d3dcompiler.h>

#include "Shader.h"

namespace sr
{
	namespace rendering
	{
		Shader::Shader(gsl::not_null<gsl::cwzstring> shaderPath) noexcept
			: mShaderBlob(nullptr)
		{
			D3DReadFileToBlob(shaderPath, &mShaderBlob);

			Ensures(mShaderBlob != nullptr);
		}

		D3D12_SHADER_BYTECODE Shader::GetShaderByteCode() const noexcept
		{
			D3D12_SHADER_BYTECODE bytecode{ mShaderBlob->GetBufferPointer(), mShaderBlob->GetBufferSize() };
			return bytecode;
		}
	}
}