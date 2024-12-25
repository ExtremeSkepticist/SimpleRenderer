#include "RootParameter.h"

namespace sr
{
	namespace rendering
	{
		RootParameter::RootParameter(const gsl::span<DescriptorRange> descriptorRanges) noexcept
			: mD3dx12DescriptorRanges({})
			, mD3dx12RootParameter({})
		{
			const size_t numDescriptorRanges = descriptorRanges.size();
			for (gsl::index i = 0; i < numDescriptorRanges; ++i)
			{
				mD3dx12DescriptorRanges.emplace_back(descriptorRanges[i].GetD3d12Range());
			}

			mD3dx12RootParameter.InitAsDescriptorTable(numDescriptorRanges, &gsl::at(mD3dx12DescriptorRanges, 0));
		}
	}
}