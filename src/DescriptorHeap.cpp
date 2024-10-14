#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		DescriptorHeap::DescriptorHeap(uint32_t numDescriptors) noexcept
			: mNumAllocatedDescriptors(0)
			, mD3d12DescriptorHeap(nullptr)
		{
			mDescriptorHeapDescription.NumDescriptors = numDescriptors;
			mDescriptorHeapDescription.NodeMask = 0;

			/*switch (descriptorHeapType)
			{
			case DescriptorHeapType::ShaderResource:
				mDescriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
				mDescriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				break;
			case DescriptorHeapType::Unset:
			default:
				assert(false);
				break;
			}*/
		}

	}
}