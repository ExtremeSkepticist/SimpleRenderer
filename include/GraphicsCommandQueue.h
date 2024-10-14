#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		enum class GraphicsCommandQueueType
		{
			Graphics,
			Compute,
			Copy,
			Num
		};

		class GraphicsCommandQueue
		{
		public:
			GraphicsCommandQueue(GraphicsCommandQueueType commandQueueType) noexcept;

			inline D3D12_COMMAND_QUEUE_DESC& GetCommandQueueDescription() noexcept
			{
				return mCommandQueueDesc;
			}

			inline ComPtr<ID3D12CommandQueue>& GetCommandQueueOwnership() noexcept
			{
				return mD3d12CommandQueue;
			}

			inline ID3D12CommandQueue* GetD3d12CommandQueue() const noexcept
			{
				return mD3d12CommandQueue.Get();
			}

		private:
			D3D12_COMMAND_QUEUE_DESC mCommandQueueDesc = {};
			ComPtr<ID3D12CommandQueue> mD3d12CommandQueue = nullptr;
		};
	}
}