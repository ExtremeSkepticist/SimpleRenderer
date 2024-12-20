#include <dxgi1_6.h>

#include "GraphicsCommandQueue.h"
#include "PlatformWindow.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		SwapChain::SwapChain(const uint8_t bufferCount, sr::platform::PlatformWindow& platformWindow, const GraphicsCommandQueue& graphicsCommandQueue) noexcept
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

			UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
			ComPtr<IDXGIFactory6> dxgiFactory = nullptr;
			CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&dxgiFactory));

			const gsl::not_null<HWND> windowHandle = platformWindow.GetWindowHandle();
			const gsl::not_null<ID3D12CommandQueue*> commandQueue = graphicsCommandQueue.GetD3d12CommandQueue();

			ComPtr<IDXGISwapChain1> tempSwapChain = nullptr;
			dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);
			dxgiFactory->CreateSwapChainForHwnd(commandQueue, windowHandle, &mSwapChainDescription, nullptr, nullptr, &tempSwapChain);
			
			const HRESULT hasConversionSucceeded = tempSwapChain.As(&mDxgiSwapChain);
			assert(SUCCEEDED(hasConversionSucceeded));
			mCurrentBufferIndex = mDxgiSwapChain->GetCurrentBackBufferIndex();

			Ensures(mDxgiSwapChain != nullptr);
		}
	}
}