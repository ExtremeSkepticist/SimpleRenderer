#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class CommandList;
		class GraphicsDevice;
		class Fence;
		enum class CommandType;

		class CommandQueue
		{
		public:
			CommandQueue(const CommandType& type) noexcept;

			void QueueSetFenceValueCommand(const Fence& fence, const uint64_t value) const noexcept;

			inline ComPtr<ID3D12CommandQueue>& GetCommandQueueOwner() noexcept
			{
				return mD3d12CommandQueue;
			}

			inline ID3D12CommandQueue* const GetD3d12CommandQueue() const noexcept
			{
				return mD3d12CommandQueue.Get();
			}

			inline CommandType GetCommandQueueType() const noexcept
			{
				return mType;
			}

			void ExecuteCommandList(const CommandList& commandList) const noexcept;

		private:
			CommandType mType;
			ComPtr<ID3D12CommandQueue> mD3d12CommandQueue = nullptr;
		};
	}
}