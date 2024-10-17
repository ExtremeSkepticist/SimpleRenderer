#include "DescriptorRange.h"

namespace sr
{
	namespace rendering
	{
		DescriptorRange::DescriptorRange(const RangeType rangeType, const uint32_t numDescriptors) noexcept
			: mRangeType(rangeType)
			, mNumDescriptors(numDescriptors)
			, mD3dx12Range({})
		{
			D3D12_DESCRIPTOR_RANGE_TYPE d3d12DescriptorRangeType;
			switch (mRangeType)
			{
				case RangeType::ConstBuffer:
					d3d12DescriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
					break;
				case RangeType::ShaderResourceBuffer:
					d3d12DescriptorRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
					break;
				default:
					assert(false);
			}

			Expects(mNumDescriptors > 0);

			mD3dx12Range.Init(d3d12DescriptorRangeType, mNumDescriptors, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
		}
	}
}