
#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"
#include "GraphicsInfrastructure.h"

namespace sr
{
	namespace rendering
	{
// Note: failure from EnumAdapterByGpuReference() and DXGI_ADAPTER_DESC1::GetDesc1() are unrecoverable
#pragma warning(push)
#pragma warning(disable : 26447)
		GraphicsDevice::GraphicsDevice(const GraphicsInfrastructure& graphicsInfrastructure) noexcept
			: mD3d12Device(nullptr)
		{
			const gsl::not_null<IDXGIFactory6*> dxgiFactory = graphicsInfrastructure.GetDxgiFactory();
			
			ComPtr<IDXGIAdapter1> adapter = nullptr;
			gsl::index adapterIndex = 0;
			while (true)
			{
				const HRESULT hasFoundAdapter = dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter));
				assert(SUCCEEDED(hasFoundAdapter));

				DXGI_ADAPTER_DESC1 adapterDesc = { 0, };
				adapter->GetDesc1(&adapterDesc);

				const bool isSoftwareAdapter = adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
				if (!isSoftwareAdapter)
				{
					if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&mD3d12Device))))
					{
						break;
					}
				}

				++adapterIndex;
			}

			Ensures(mD3d12Device != nullptr);
		}
#pragma warning(pop)

		void GraphicsDevice::CreateCommandQueue(GraphicsCommandQueue& commandQueue, const GraphicsInfrastructure& graphicsInfrastructure)
		{
			const D3D12_COMMAND_QUEUE_DESC& description = commandQueue.GetCommandQueueDescription();
			ComPtr<ID3D12CommandQueue>& commandQueueOwnerShip = commandQueue.GetCommandQueueOwnership();

			mD3d12Device->CreateCommandQueue(&description, IID_PPV_ARGS(&commandQueueOwnerShip));
			
			Ensures(commandQueueOwnerShip != nullptr);
		}

	}
}