#include "RootSignature.h"

namespace sr
{
	namespace rendering
	{
		RootSignature::RootSignature(const uint32_t rootParameterNum, const uint32_t descriptorRangeNum)
			: mRootSignatureFlags(D3D12_ROOT_SIGNATURE_FLAGS::D3D12_ROOT_SIGNATURE_FLAG_NONE)
			, mSamplerDesc({})
			, mRootParameters()
			, mDescriptorRanges()
			, mRootSignature(nullptr)
		{
			mRootParameters.reserve(rootParameterNum);
			mDescriptorRanges.reserve(descriptorRangeNum);
		}

		//void RootSignature::AddDescriptorRange(const RootSignatureDescriptorType descriptorType, const uint32_t numDescriptors) noexcept
		//{
		//	const gsl::index rangeIndex = mDescriptorRanges.size();
		//	mDescriptorRanges.emplace_back();

		//	D3D12_DESCRIPTOR_RANGE_TYPE d3d12DescriptorRangeType;
		//	switch (descriptorType)
		//	{
		//	case RootSignatureDescriptorType::ConstBuffer:
		//		d3d12DescriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		//		break;
		//	case RootSignatureDescriptorType::ShaderResourceBuffer:
		//		d3d12DescriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		//		break;
		//	default:
		//		assert(false);
		//	}

		//	mDescriptorRanges[rangeIndex].Init(d3d12DescriptorRangeType, numDescriptors, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		//}
	}
}