#include <d3dx12.h>

#include "GpuResource.h"
#include "GraphicsCommandList.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		GraphicsCommandList::GraphicsCommandList(const GraphicsDevice& graphicsDevice) noexcept
			: mD3d12CommandAllocator(nullptr)
			, mD3d12CommandList(nullptr)
		{
			graphicsDevice.CreateCommandList(*this);
		}

		void GraphicsCommandList::ResetCommandList() noexcept
		{
			mD3d12CommandAllocator->Reset();
			mD3d12CommandList->Reset(mD3d12CommandAllocator.Get(), nullptr);
		}

		void GraphicsCommandList::RecordCommandCopyResource(GpuResource& destResource, const GpuResource& srcResource, const D3D12_SUBRESOURCE_DATA& subResourceData) noexcept
		{
			const auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				destResource.GetD3d12Resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			mD3d12CommandList->ResourceBarrier(1, &transitionBarrier);
			
			UpdateSubresources(mD3d12CommandList.Get(), destResource.GetD3d12Resource(), srcResource.GetD3d12Resource(), 0, 0, 1, &subResourceData); // GraphicsDevice

			mD3d12CommandList->Close();
		}

	}
}