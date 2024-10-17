#include "RootParameter.h"

namespace sr
{
	namespace rendering
	{
		RootParameter::RootParameter(const uint32_t rootParameterNum) noexcept
			: mDescriptorRanges({})
			, mD3dx12RootParameter({})
		{
			mDescriptorRanges.reserve(rootParameterNum);
		}

		void RootParameter::AddDescriptorRange(const DescriptorRange::RangeType rangeType, const uint32_t numDescriptors) noexcept
		{
			mDescriptorRanges.emplace_back(rangeType, numDescriptors);
		}
	}
}