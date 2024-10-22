#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		DescriptorHeap::DescriptorHeap(const uint32_t numDescriptors) noexcept
			: mD3d12DescriptorHeap(nullptr)
			, mGpuResources(numDescriptors)
		{
			mDescriptorHeapDescription.NumDescriptors = numDescriptors;
			mDescriptorHeapDescription.NodeMask = 0;

			assert(mGpuResources.size() == numDescriptors);
		}

	}
}