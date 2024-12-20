#include "Fence.h"
#include "GraphicsCommandList.h"
#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"
#include "PlatformWindow.h"
#include "RenderingPipeline.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		// TODO: remove const_cast
		RenderingPipeline::RenderingPipeline(const platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum, const uint32_t infrastructureFlag) noexcept
			: mGraphicsDevice(make_unique<GraphicsDevice>())
			, mGraphicsCommandQueue(make_unique<GraphicsCommandQueue>(GraphicsCommandQueueType::Graphics, *mGraphicsDevice))
			, mGraphicsCommandList(make_unique<GraphicsCommandList>(*mGraphicsDevice))
			, mSwapChain(make_unique<SwapChain>(swapChainBufferNum, const_cast<sr::platform::PlatformWindow&>(platformWindow), *mGraphicsCommandQueue))
			, mFence(make_unique<Fence>(*mGraphicsDevice))
		{
			Ensures(mGraphicsDevice != nullptr);
			Ensures(mGraphicsCommandQueue != nullptr);
			Ensures(mGraphicsCommandList != nullptr);
			Ensures(mSwapChain != nullptr);
			Ensures(mFence != nullptr);
		}

	}
}