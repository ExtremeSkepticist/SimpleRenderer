#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		DescriptorHeap::DescriptorHeap() noexcept
			: mDesc({})
			, mD3d12DescriptorHeap(nullptr)
			, mCapacity(defaultNumDescriptor)
			, mSize(0)
		{
			mDesc.NumDescriptors = mCapacity;
			mDesc.NodeMask = 0;
		}

	}
}