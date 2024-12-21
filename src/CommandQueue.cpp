#include "Fence.h"
#include "GraphicsCommandList.h"
#include "CommandQueue.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		CommandQueue::CommandQueue(CommandQueueType type) noexcept
			: mCommandQueueDesc({})
			, mD3d12CommandQueue(nullptr)
		{
			switch (type)
			{
				case CommandQueueType::Graphics:
					mCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
					break;
				case CommandQueueType::Compute:
					mCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
					break;
				case CommandQueueType::Copy:
					mCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
					break;
				default:
					assert(false);
					break;
			}
		}

		void CommandQueue::QueueSetFenceValueCommand(const Fence& fence, const uint64_t value) const noexcept
		{
			ID3D12Fence* d3d12Fence = fence.GetD3d12Fence();
			mD3d12CommandQueue->Signal(d3d12Fence, value);
		}
	
		void CommandQueue::ExecuteCommandList(GraphicsCommandList& graphicsCommandList) const noexcept
		{
			ID3D12CommandList* arr[] = { graphicsCommandList.GetGraphicsCommandList() };
			mD3d12CommandQueue->ExecuteCommandLists(1, arr);
		}
	}
}