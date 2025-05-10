#pragma once

#include "Common.h"

namespace sr
{
	namespace platform
	{
		class PlatformWindow;
	}

	namespace rendering
	{
		class CommandList;
		class CommandQueue;
		class Fence;
		class FrameBuilder;
		class GraphicsDevice;
		class SwapChain;
		class RenderTargetDescriptorHeap;

		class RenderingPipeline final
		{
		public:
			RenderingPipeline(platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum) noexcept;
			RenderingPipeline() = delete;
			RenderingPipeline(const RenderingPipeline&) = delete;
			RenderingPipeline(RenderingPipeline&&) = delete;
			~RenderingPipeline() = default;

			RenderingPipeline& operator=(const RenderingPipeline&) = delete;
			RenderingPipeline& operator=(RenderingPipeline&&) = delete;

			void Render(FrameBuilder& FrameBuilder);

			inline GraphicsDevice& GetGraphicsDevice() noexcept
			{
				return *mGraphicsDevice;
			}

			inline CommandQueue& GetGraphicsCommandQueue() noexcept
			{
				return *mGraphicsCommandQueue;
			}

			inline CommandList& GetGraphicsCommandList() noexcept
			{
				return *mGraphicsCommandList;
			}

			inline SwapChain& GetSwapChain() noexcept
			{
				return *mSwapChain;
			}

			void WaitForPreviousFrame() noexcept;

		private:
			void setupDefaultCommands(FrameBuilder& FrameBuilder);

			unique_ptr<GraphicsDevice> mGraphicsDevice = nullptr;
			unique_ptr<CommandQueue> mGraphicsCommandQueue = nullptr;
			unique_ptr<CommandList> mGraphicsCommandList = nullptr;
			unique_ptr<SwapChain> mSwapChain = nullptr;
			unique_ptr<Fence> mFence = nullptr;
		};
	}
}