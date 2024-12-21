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
		RenderTargetDescriptorHeap::RenderTargetDescriptorHeap(const uint32_t numDescriptors, SwapChain& swapChain) noexcept
			: DescriptorHeap(numDescriptors)
		{
			D3D12_DESCRIPTOR_HEAP_DESC& heapDescription = DescriptorHeap::GetDescription();
			heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			graphicsDevice.CreateDescriptorHeap(*this);

			const uint8_t bufferCount = swapChain.GetBufferCount();
			gsl::not_null<IDXGISwapChain3*> const dxgiSwapChain = swapChain.GetDxgiSwapChain();
			std::vector<GpuResource>& gpuResources = GetGpuResources();
			assert(gpuResources.size() > 0);
			
			for (gsl::index i = 0; i < bufferCount; ++i)
			{
				GpuResource& resource = gsl::at(gpuResources, i);
				ComPtr<ID3D12Resource>& d3d12ResourceOwner = resource.GetD3d12ResourceOwner();
				dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&d3d12ResourceOwner));
				{
					// TODO: replace this with better solution
					std::wstring name(L"Render Target buffer ");
					name += i;
					resource.SetResourceName(name.c_str());

					resource.SetResourceDescription(CD3DX12_RESOURCE_DESC(d3d12ResourceOwner->GetDesc()));
					
					CD3DX12_HEAP_PROPERTIES heapProperties = {};
					D3D12_HEAP_FLAGS heapFlag;
					d3d12ResourceOwner->GetHeapProperties(&heapProperties, &heapFlag);
					resource.SetResourceHeapProperties(heapProperties);

					resource.SetResourceState(ResourceState::Common);
				}
				
				graphicsDevice.CreateRenderTargetView(*this, i);
			}
		}
	}
}