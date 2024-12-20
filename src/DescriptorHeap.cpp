#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		DescriptorHeap::DescriptorHeap(const uint32_t numDescriptors) noexcept
			: mD3d12DescriptorHeap(nullptr)
			, mNumDescriptors(numDescriptors)
		{
			//TODO: find a better way to do this
			mGpuResources.reserve(numDescriptors);
			for (size_t i = 0; i < numDescriptors; ++i)
			{
				mGpuResources.emplace_back();
			}

			mDescriptorHeapDescription.NumDescriptors = mNumDescriptors;
			mDescriptorHeapDescription.NodeMask = 0;

			assert(mGpuResources.size() == mNumDescriptors);
		}

	}
}