#include "GraphicsDevice.h"
#include "RootSignature.h"

namespace sr
{
	namespace rendering
	{
		RootSignature::RootSignature(const gsl::span<RootParameter> rootParameters) noexcept
			: mSerializedRootSignature(nullptr)
			, mD3d12RootSignature(nullptr)
		{
			D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
			{
				samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
				samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
				samplerDesc.MaxAnisotropy = 16;
				samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
				samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
				samplerDesc.MinLOD = 0;
				samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
				samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			}

			constexpr D3D12_ROOT_SIGNATURE_FLAGS rootSignautureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			const size_t numRootParameters = rootParameters.size();

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
			rootSignatureDesc.Init_1_1(numRootParameters, &rootParameters[0].GetD3dx12RootParameter(), 1, &samplerDesc, rootSignautureFlags);

			D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &mSerializedRootSignature, nullptr);
			Ensures(mSerializedRootSignature!= nullptr);
		}
	}
}