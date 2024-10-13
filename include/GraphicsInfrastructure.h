#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsInfrastructure
		{
			friend class SwapChain;

		public:
			GraphicsInfrastructure(UINT flag) noexcept;

			GraphicsInfrastructure() = delete;
			GraphicsInfrastructure(const GraphicsInfrastructure&) = delete;
			GraphicsInfrastructure(GraphicsInfrastructure&&) = delete;
			GraphicsInfrastructure& operator=(const GraphicsInfrastructure&) = delete;
			GraphicsInfrastructure& operator=(GraphicsInfrastructure&&) = delete;
			~GraphicsInfrastructure() = default;

			inline IDXGIFactory6* GetDxgiFactory() const noexcept
			{
				return mDxgiFactory.Get();
			}

		private:
			ComPtr<IDXGIFactory6> mDxgiFactory = nullptr;
		};
	}
}