#include "GraphicsInfrastructure.h"
#include "PlatformWindow.h"
#include "SwapChain.h"

namespace sr
{
	namespace rendering
	{
		GraphicsInfrastructure::GraphicsInfrastructure(UINT flag) noexcept
			: mDxgiFactory(nullptr)
		{
#if defined(_DEBUG)
			ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
				flag |= DXGI_CREATE_FACTORY_DEBUG;
			}
#else
			constexpr UINT dxgiFactoryFlags = 0;
#endif

			CreateDXGIFactory2(flag, IID_PPV_ARGS(&mDxgiFactory));
			
			Ensures(mDxgiFactory != nullptr);
		}
	}
}