#include <d3dx12.h>
#include <format>

#include "GpuResource.h"
#include "GraphicsDevice.h"
#include "RenderTargetDescriptorHeap.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		RenderTargetDescriptorHeap::RenderTargetDescriptorHeap() noexcept
			: DescriptorHeap()
		{
			D3D12_DESCRIPTOR_HEAP_DESC& heapDescription = DescriptorHeap::GetDescription();
			heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		}

		DescriptorHandle RenderTargetDescriptorHeap::GetHeapStartHandle() const noexcept
		{
			DescriptorHandle handle = {
				GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart()
				, 0
			};

			return handle;
		}
	}
}