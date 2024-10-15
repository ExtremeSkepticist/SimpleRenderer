#include <d3dx12.h>
#include <optional>

#include "GpuResource.h"
#include "GraphicsDevice.h"
#include "RenderTargetDescriptorHeap.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(const uint32_t numDescriptors
			, SwapChain& swapChain, const GraphicsDevice& graphicsDevice) noexcept
			: DescriptorHeap(numDescriptors)
		{
			D3D12_DESCRIPTOR_HEAP_DESC& heapDescription = DescriptorHeap::GetDescription();
			heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			graphicsDevice.CreateDescriptorHeap(*this);

			const uint8_t bufferCount = swapChain.GetBufferCount();
			gsl::not_null<IDXGISwapChain3*> const dxgiSwapChain = swapChain.GetDxgiSwapChain();
			std::vector<GpuResource>& gpuResources = GetGpuResources();
			for (gsl::index i = 0; i < bufferCount; ++i)
			{
				ComPtr<ID3D12Resource>& d3d12ResourceOwner = gsl::at(gpuResources, i).GetD3d12ResourceOwner();
				dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&d3d12ResourceOwner));
				graphicsDevice.CreateRenderTargetView(*this, i);
			}
		}
	}
}