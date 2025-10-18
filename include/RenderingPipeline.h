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
		class RenderableNode;
		class RenderTargetDescriptorHeap;
		class Mesh;

		class RenderingPipeline final
		{
		public:
			RenderingPipeline(platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum) noexcept;
			RenderingPipeline() = delete;
			RenderingPipeline(const RenderingPipeline&) = delete;
			RenderingPipeline(RenderingPipeline&&) = delete;
			~RenderingPipeline();

			RenderingPipeline& operator=(const RenderingPipeline&) = delete;
			RenderingPipeline& operator=(RenderingPipeline&&) = delete;

			void RenderStart(FrameBuilder& frameBuilder);
			void RenderEnd();
			void Render(FrameBuilder& frameBuilder, Mesh& node);

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
			void setupDefaultCommands(FrameBuilder& frameBuilder, Mesh& mesh);

			unique_ptr<GraphicsDevice> mGraphicsDevice = nullptr;
			unique_ptr<CommandQueue> mGraphicsCommandQueue = nullptr;
			unique_ptr<CommandList> mGraphicsCommandList = nullptr;
			unique_ptr<SwapChain> mSwapChain = nullptr;
			unique_ptr<Fence> mFence = nullptr;
		};
	}
}