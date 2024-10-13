#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		GraphicsCommandQueue::GraphicsCommandQueue(GraphicsCommandQueueType commandQueueType) noexcept
			: mCommandQueueDesc({})
			, mD3d12CommandQueue(nullptr)
		{
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
		}
	}
}