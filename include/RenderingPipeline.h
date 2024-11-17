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
		class Fence;
		class GraphicsCommandList;
		class GraphicsCommandQueue;
		class GraphicsDevice;
		class GraphicsInfrastructure;
		class SwapChain;

		class RenderingPipeline final
		{
		public:
			RenderingPipeline() = delete;
			RenderingPipeline(const RenderingPipeline&) = delete;
			RenderingPipeline(RenderingPipeline&&) = delete;
			RenderingPipeline(const platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum, const uint32_t infrastructureFlag) noexcept;
			~RenderingPipeline() = default;

			RenderingPipeline& operator=(const RenderingPipeline&) = delete;
			RenderingPipeline& operator=(RenderingPipeline&&) = delete;

			inline GraphicsDevice& GetGraphicsDevice() noexcept
			{
				Expects(mGraphicsDevice != nullptr);
				return *mGraphicsDevice;
			}

			inline GraphicsCommandQueue& GetGraphicsCommandQueue() noexcept
			{
				Expects(mGraphicsCommandQueue != nullptr);
				return *mGraphicsCommandQueue;
			}

			inline GraphicsCommandList& GetGraphicsCommandList() noexcept
			{
				Expects(mGraphicsCommandList != nullptr);
				return *mGraphicsCommandList;
			}

			inline SwapChain& GetSwapChain() noexcept
			{
				Expects(mSwapChain != nullptr);
				return *mSwapChain;
			}

		private:
			unique_ptr<GraphicsInfrastructure> mGraphicsInfrastructure = nullptr;
			unique_ptr<GraphicsDevice> mGraphicsDevice = nullptr;
			unique_ptr<GraphicsCommandQueue> mGraphicsCommandQueue = nullptr;
			unique_ptr<GraphicsCommandList> mGraphicsCommandList = nullptr;
			unique_ptr<SwapChain> mSwapChain = nullptr;
			unique_ptr<Fence> mFence = nullptr;
		};
	}
}