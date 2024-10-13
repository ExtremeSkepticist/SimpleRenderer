#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Common.h"

namespace sr
{
	class platform::PlatformWindow;

	namespace rendering
	{
		class GraphicsInfrastructure;
		class GraphicsCommandQueue;
		
		class SwapChain
		{

		public:
			SwapChain() = delete;
			SwapChain(const uint8_t bufferCount, sr::platform::PlatformWindow& platformWindow
				, const GraphicsCommandQueue& graphicsCommandQueue, const GraphicsInfrastructure& graphicsInfrastructure) noexcept;
			SwapChain(const SwapChain&) = delete;
			SwapChain(SwapChain&&) = delete;
			SwapChain& operator=(const SwapChain&) = delete;
			SwapChain& operator=(SwapChain&&) = delete;
			~SwapChain() = default;

			inline IDXGISwapChain3* GetDxgiSwapChain() const noexcept
			{
				return mDxgiSwapChain.Get();
			}

		private:
			uint8_t mBufferCount = 0;
			uint16_t mBufferWidth = 0;
			uint16_t mBufferHeight = 0;

			ComPtr<IDXGISwapChain3> mDxgiSwapChain = nullptr;
			DXGI_SWAP_CHAIN_DESC1 mSwapChainDescription = { 0, };
			uint32_t mCurrentBufferIndex = 0;
		};
	}
}
