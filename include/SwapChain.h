#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Common.h"

namespace sr
{
	namespace platform
	{
		class PlatformWindow;
	};
	
	namespace rendering
	{
		class CommandQueue;
		
		class SwapChain
		{

		public:
			SwapChain(const uint8_t bufferCount, sr::platform::PlatformWindow& platformWindow, const CommandQueue& graphicsCommandQueue) noexcept;
			SwapChain() = delete;
			SwapChain(const SwapChain&) = delete;
			SwapChain(SwapChain&&) = delete;
			SwapChain& operator=(const SwapChain&) = delete;
			SwapChain& operator=(SwapChain&&) = delete;
			~SwapChain() = default;

			inline uint8_t GetBufferCount() const noexcept
			{
				return mBufferCount;
			}

			inline uint16_t GetBufferWidth() const noexcept
			{
				return mBufferWidth;
			}
			
			inline uint16_t GetBufferHeight() const noexcept
			{
				return mBufferHeight;
			}

			inline IDXGISwapChain3* const GetDxgiSwapChain() const noexcept
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
