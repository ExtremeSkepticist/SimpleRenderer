#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsCommandList;
		class GraphicsDevice;
		class Fence;

		enum class CommandQueueType
		{
			Graphics,
			Compute,
			Copy,
			Num
		};

		// TODO: Change name to GraphicsCommandQueue -> CommandQueue
		class CommandQueue
		{
		public:
			CommandQueue(CommandQueueType type) noexcept;

			void QueueSetFenceValueCommand(const Fence& fence, const uint64_t value) const noexcept;

			inline D3D12_COMMAND_QUEUE_DESC& GetCommandQueueDescription() noexcept
			{
				return mCommandQueueDesc;
			}

			inline ComPtr<ID3D12CommandQueue>& GetCommandQueueOwnership() noexcept
			{
				return mD3d12CommandQueue;
			}

			inline ID3D12CommandQueue* const GetD3d12CommandQueue() const noexcept
			{
				return mD3d12CommandQueue.Get();
			}

			void ExecuteCommandList(GraphicsCommandList& graphicsCommandList) const noexcept;

		private:
			D3D12_COMMAND_QUEUE_DESC mCommandQueueDesc = {};
			ComPtr<ID3D12CommandQueue> mD3d12CommandQueue = nullptr;
		};
	}
}