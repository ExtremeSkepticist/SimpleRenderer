#pragma once

#include <d3d12.h>
#include <d3dx12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		// TODO: break down this into RootParameter and DescriptorRange class
		class RootSignature
		{
		public:
			RootSignature(const uint32_t rootParameterNum, const uint32_t descriptorRangeNum) noexcept;
			~RootSignature() = default;
			RootSignature() = delete;

			//void AddDescriptorRange(const RootSignatureDescriptorType descriptorType, const uint32_t numDescriptors) noexcept;

		private:
			D3D12_ROOT_SIGNATURE_FLAGS mRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE;
			D3D12_STATIC_SAMPLER_DESC mSamplerDesc = {};
			std::vector<CD3DX12_DESCRIPTOR_RANGE1> mDescriptorRanges = {};
			std::vector<CD3DX12_ROOT_PARAMETER1> mRootParameters = {};
			ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
		};
	}
}