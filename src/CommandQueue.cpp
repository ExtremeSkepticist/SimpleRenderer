#include "Fence.h"
#include "CommandList.h"
#include "CommandType.h"
#include "CommandQueue.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		CommandQueue::CommandQueue(const CommandType& type) noexcept
			: mType(type)
			, mD3d12CommandQueue(nullptr)
		{
			switch (type)
			{
				case CommandType::Graphics:
				case CommandType::Copy:
				case CommandType::Compute:
					break;
				case CommandType::Num:
				default:
					assert(false);
					break;
			}
		}

		void CommandQueue::QueueSetFenceValueCommand(const Fence& fence, const uint64_t value) const noexcept
		{
			Expects(mD3d12CommandQueue != nullptr);

			ID3D12Fence* d3d12Fence = fence.GetD3d12Fence();
			mD3d12CommandQueue->Signal(d3d12Fence, value);
		}
	
		void CommandQueue::ExecuteCommandList(const CommandList& commandList) const noexcept
		{
			Expects(mD3d12CommandQueue != nullptr);

			ID3D12CommandList* arr[] = { commandList.GetCommandList() };
			mD3d12CommandQueue->ExecuteCommandLists(1, &gsl::at(arr, 0));
		}
	}
}