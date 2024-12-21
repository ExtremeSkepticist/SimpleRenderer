#include "Fence.h"
#include "GraphicsCommandList.h"
#include "CommandQueue.h"
#include "GraphicsDevice.h"
#include "PlatformWindow.h"
#include "RenderingPipeline.h"
#include "RenderTargetDescriptorHeap.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		// TODO: Use Device to create d3d12 object, outside its constructor
		// TODO: Redesign the following:
		//	2. GraphicsCommandList -> CommandList: Separate command allocator? Could just pass different allocators that are not in recording
		//  3. DescriptorHeap -> Separate resources from DescriptorHeap. Resources should go into Input struct/class and get bound later.
		//	Refer to complete code; maybe have Gpu descriptor heap in RenderingPipeline class and copy from cpu descriptor heap in Update(
		RenderingPipeline::RenderingPipeline(platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum) noexcept
			: mGraphicsDevice(make_unique<GraphicsDevice>())
			, mGraphicsCommandQueue(make_unique<CommandQueue>(CommandQueueType::Graphics))
			, mGraphicsCommandList(make_unique<GraphicsCommandList>(*mGraphicsDevice))
			, mSwapChain(make_unique<SwapChain>(swapChainBufferNum, platformWindow, *mGraphicsCommandQueue))
			, mRenderTargetDescriptorHeap(make_unique<RenderTargetDescriptorHeap>(swapChainBufferNum))
			, mFence(make_unique<Fence>())
		{
			mGraphicsDevice->CreateCommandQueue(*mGraphicsCommandQueue);
			mGraphicsDevice->CreateCommandList(*mGraphicsCommandList);
			mGraphicsDevice->CreateFence(*mFence);
		}

		void RenderingPipeline::Update()
		{

		}
	}
}