#pragma once

#include <d3d12.h>
#include <d3dx12.h>

#include "Common.h"
#include "RootParameter.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		class RootSignature
		{
		public:
			RootSignature(const gsl::span<RootParameter> rootParameters, const GraphicsDevice& graphicsDevice) noexcept;
			~RootSignature() = default;
			RootSignature() = delete;

			inline ID3DBlob* GetSerializedRootSignature() const noexcept
			{
				return mSerializedRootSignature.Get();
			}

			inline ID3D12RootSignature* GetD3d12RootSignature() const noexcept
			{
				return mD3d12RootSignature.Get();
			}

			inline ComPtr<ID3D12RootSignature>& GetD3d12RootSignatureOwner() noexcept
			{
				return mD3d12RootSignature;
			}

		private:
			std::vector<CD3DX12_ROOT_PARAMETER1> mD3dx12RootParameters;
			ComPtr<ID3DBlob> mSerializedRootSignature = nullptr;
			ComPtr<ID3D12RootSignature> mD3d12RootSignature = nullptr;
		};
	}
}