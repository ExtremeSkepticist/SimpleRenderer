#include "RenderTargetDescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(uint32_t numDescriptors) noexcept
			: DescriptorHeap(numDescriptors)
		{
			D3D12_DESCRIPTOR_HEAP_DESC& heapDescription = DescriptorHeap::GetDescription();
			heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		}
	}
}