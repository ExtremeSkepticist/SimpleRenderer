#include "Fence.h"
#include "GraphicsCommandList.h"
#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		GraphicsCommandQueue::GraphicsCommandQueue(GraphicsCommandQueueType commandQueueType, const GraphicsDevice& graphicsDevice) noexcept
			: mCommandQueueDesc({})
			, mD3d12CommandQueue(nullptr)
		{
			Expects(mD3d12CommandQueue == nullptr);

			switch (commandQueueType)
			{
				case GraphicsCommandQueueType::Graphics:
					mCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
					break;
				case GraphicsCommandQueueType::Compute:
					mCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
					break;
				case GraphicsCommandQueueType::Copy:
					mCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
					break;
				default:
					assert(false);
					break;
			}

			graphicsDevice.CreateCommandQueue(*this);

			Ensures(mD3d12CommandQueue != nullptr);
		}

		void GraphicsCommandQueue::QueueSetFenceValueCommand(const Fence& fence, const uint64_t value) const noexcept
		{
			ID3D12Fence* d3d12Fence = fence.GetD3d12Fence();
			mD3d12CommandQueue->Signal(d3d12Fence, value);
		}
	
		void GraphicsCommandQueue::ExecuteCommandList(GraphicsCommandList& graphicsCommandList) const noexcept
		{
			ID3D12CommandList* arr[] = { graphicsCommandList.GetGraphicsCommandList() };
			mD3d12CommandQueue->ExecuteCommandLists(1, arr);
		}
	}
}