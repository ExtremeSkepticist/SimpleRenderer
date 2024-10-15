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