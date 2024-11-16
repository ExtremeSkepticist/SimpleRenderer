#include "Fence.h"
#include "GraphicsCommandList.h"
#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"
#include "GraphicsInfrastructure.h"
#include "PlatformWindow.h"
#include "RenderingPipeline.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		RenderingPipeline::RenderingPipeline(const platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum, const uint32_t infrastructureFlag) noexcept
			: mGraphicsInfrastructure(make_unique<GraphicsInfrastructure>(infrastructureFlag))
			, mGraphicsDevice(make_unique<GraphicsDevice>(*mGraphicsInfrastructure))
			, mGraphicsCommandQueue(make_unique<GraphicsCommandQueue>(GraphicsCommandQueueType::Graphics, *mGraphicsDevice))
			, mGraphicsCommandList(make_unique<GraphicsCommandList>(*mGraphicsDevice))
			, mSwapChain(make_unique<SwapChain>(swapChainBufferNum, platformWindow, *mGraphicsCommandQueue, *mGraphicsInfrastructure))
			, mFence(make_unique<Fence>(*mGraphicsDevice))
		{
			Ensures(mGraphicsInfrastructure != nullptr);
			Ensures(mGraphicsDevice != nullptr);
			Ensures(mGraphicsCommandQueue != nullptr);
			Ensures(mGraphicsCommandList != nullptr);
			Ensures(mSwapChain != nullptr);
			Ensures(mFence != nullptr);
		}

	}
}