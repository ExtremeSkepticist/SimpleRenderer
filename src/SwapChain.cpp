#include "GraphicsCommandQueue.h"
#include "GraphicsInfrastructure.h"
#include "PlatformWindow.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		SwapChain::SwapChain(const uint8_t bufferCount, sr::platform::PlatformWindow& platformWindow
			, const GraphicsCommandQueue& graphicsCommandQueue, const GraphicsInfrastructure& graphicsInfrastructure) noexcept
			: mBufferCount(bufferCount)
			, mBufferWidth(platformWindow.GetFrameWidth())
			, mBufferHeight(platformWindow.GetFrameHeight())
			, mDxgiSwapChain(nullptr)
			, mCurrentBufferIndex(0)
		{
			//Expects(mBufferCount > 0);
			//Expects(mBufferWidth > 0);
			//Expects(mBufferHeight > 0);
			
			mSwapChainDescription.BufferCount = mBufferCount;
			mSwapChainDescription.Width = mBufferWidth;
			mSwapChainDescription.Height = mBufferHeight;
			mSwapChainDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			mSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			mSwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			mSwapChainDescription.SampleDesc.Count = 1;

			const gsl::not_null<IDXGIFactory6*> dxgiFactory = graphicsInfrastructure.GetDxgiFactory();
			const gsl::not_null<HWND> windowHandle = platformWindow.GetWindowHandle();
			const gsl::not_null<ID3D12CommandQueue*> commandQueue = graphicsCommandQueue.GetD3d12CommandQueue();

			ComPtr<IDXGISwapChain1> temp = nullptr;
			dxgiFactory->MakeWindowAssociation(windowHandle, DXGI_MWA_NO_ALT_ENTER);
			dxgiFactory->CreateSwapChainForHwnd(commandQueue, windowHandle, &mSwapChainDescription, nullptr, nullptr, &temp);
			assert(temp != nullptr);

			const HRESULT hasConversionSucceeded = temp.As(&mDxgiSwapChain);
			assert(SUCCEEDED(hasConversionSucceeded));
			mCurrentBufferIndex = mDxgiSwapChain->GetCurrentBackBufferIndex();

			Ensures(mDxgiSwapChain != nullptr);
		}
	}
}