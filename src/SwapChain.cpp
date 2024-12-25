#include <dxgi1_6.h>

#include "CommandQueue.h"
#include "PlatformWindow.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		SwapChain::SwapChain(const uint8_t bufferCount, sr::platform::PlatformWindow& platformWindow) noexcept
			: mBufferCount(bufferCount)
			, mBufferWidth(platformWindow.GetFrameWidth())
			, mBufferHeight(platformWindow.GetFrameHeight())
			, mDxgiSwapChain(nullptr)
			, mCurrentBufferIndex(0)
		{
			Expects(mBufferCount > 0);
			
			mSwapChainDescription.BufferCount = mBufferCount;
			mSwapChainDescription.Width = mBufferWidth;
			mSwapChainDescription.Height = mBufferHeight;
			mSwapChainDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			mSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			mSwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			mSwapChainDescription.SampleDesc.Count = 1;
		}
	}
}