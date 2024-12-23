#include "Fence.h"
#include "CommandList.h"
#include "CommandType.h"
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
		// TODO: Separeate SwapChain and DescriptorHeap from Pipeline. They should be FrameResource, and pipeline should be just "tools"
		// TODO: Use Device to create d3d12 object, outside its constructor
		// TODO: investigate scene graph
		//  - Separate resources from DescriptorHeap. Resources should go into Input struct/class and get bound later.
		// TODO: Redesign the following:
		//	Consider separating command allocator from command list? Could just pass different allocators that are not in recording
		//	Refer to complete code; maybe have Gpu descriptor heap in RenderingPipeline class and copy from cpu descriptor heap in Update(
		RenderingPipeline::RenderingPipeline(platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum) noexcept
			: mGraphicsDevice(make_unique<GraphicsDevice>())
			, mGraphicsCommandQueue(make_unique<CommandQueue>(CommandType::Graphics))
			, mGraphicsCommandList(make_unique<CommandList>(CommandType::Graphics))
			, mSwapChain(make_unique<SwapChain>(swapChainBufferNum, platformWindow, *mGraphicsCommandQueue))
			, mFence(make_unique<Fence>())
			, mFrameResource(make_unique<FrameResource>())
		{
			mGraphicsDevice->CreateCommandQueue(*mGraphicsCommandQueue);
			mGraphicsDevice->CreateCommandList(*mGraphicsCommandList);
			mGraphicsDevice->CreateFence(*mFence);
		}

		void RenderingPipeline::Update(const FrameResource& frameResource)
		{

		}
	}
}