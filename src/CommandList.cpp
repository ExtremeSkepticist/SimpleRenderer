#include <d3dx12.h>

#include "GpuResource.h"
#include "CommandType.h"
#include "CommandList.h"
#include "GraphicsDevice.h"
#include "GraphicsPipelineState.h"
#include "RootSignature.h"

namespace sr
{
	namespace rendering
	{
		CommandList::CommandList(const CommandType& type) noexcept
			: mType(type)
		{
			switch (mType)
			{
			case CommandType::Graphics:
			case CommandType::Copy:
			case CommandType::Compute:
				break;
			default:
				assert(false);
				break;
			}

			Ensures(type != CommandType::Num);
		}

		void CommandList::ResetCommandList() noexcept
		{
			mD3d12CommandAllocator->Reset();
			mD3d12CommandList->Reset(mD3d12CommandAllocator.Get(), nullptr);
		}

		void CommandList::RecordCommandCopyResource(GpuResource& destResource, const GpuResource& srcResource, const D3D12_SUBRESOURCE_DATA& subResourceData) noexcept
		{
			UpdateSubresources(mD3d12CommandList.Get(), destResource.GetD3d12Resource(), srcResource.GetD3d12Resource(), 0, 0, 1, &subResourceData); // GraphicsDevice
			
			const auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				destResource.GetD3d12Resource(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
			mD3d12CommandList->ResourceBarrier(1, &transitionBarrier);
		}

		void CommandList::SetPipelineState(const GraphicsPipelineState& graphicsPipelineState) const noexcept
		{
			Expects(mD3d12CommandList != nullptr);

			mD3d12CommandList->SetPipelineState(graphicsPipelineState.GetPipelineState());
		}
		
		void CommandList::SetGraphicsRootSignature(const RootSignature& rootSignature) const noexcept
		{
			Expects(mD3d12CommandList != nullptr);

			mD3d12CommandList->SetGraphicsRootSignature(rootSignature.GetD3d12RootSignature());
		}

	}
}