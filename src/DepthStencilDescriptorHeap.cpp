#include <d3dx12.h>

#include <DirectXTex.h>
#include "GpuResource.h"
#include "GraphicsDevice.h"
#include "DepthStencilDescriptorHeap.h"
#include "SwapChain.h"
#include "Texture.h"

namespace sr
{
	namespace rendering
	{
		DepthStencilDescriptorHeap::DepthStencilDescriptorHeap() noexcept
			: DescriptorHeap()
		{
			D3D12_DESCRIPTOR_HEAP_DESC& heapDescription = DescriptorHeap::GetDescription();
			heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		}

		DescriptorHandle DepthStencilDescriptorHeap::GetHeapStartHandle() const noexcept
		{
			DescriptorHandle handle = {
				GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart()
				, 0
			};

			return handle;
		}

	}
}