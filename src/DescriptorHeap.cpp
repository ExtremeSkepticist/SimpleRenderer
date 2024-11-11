#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		DescriptorHeap::DescriptorHeap(const uint32_t numDescriptors) noexcept
			: mD3d12DescriptorHeap(nullptr)
			, mNumDescriptors(numDescriptors)
			, mGpuResources(mNumDescriptors)
		{
			mDescriptorHeapDescription.NumDescriptors = mNumDescriptors;
			mDescriptorHeapDescription.NodeMask = 0;

			assert(mGpuResources.size() == mNumDescriptors);
		}

	}
}