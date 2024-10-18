
#include "DescriptorHeap.h"
#include "GpuResource.h"
#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"
#include "GraphicsInfrastructure.h"
#include "RenderTargetDescriptorHeap.h"
#include "RootSignature.h"

namespace sr
{
	namespace rendering
	{
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

		void GraphicsDevice::CreateCommandQueue(GraphicsCommandQueue& commandQueue, const GraphicsInfrastructure& graphicsInfrastructure) const noexcept
		{
			ComPtr<ID3D12CommandQueue>& commandQueueOwnerShip = commandQueue.GetCommandQueueOwnership();

			Expects(commandQueueOwnerShip == nullptr);

			mD3d12Device->CreateCommandQueue(&commandQueue.GetCommandQueueDescription()
				, IID_PPV_ARGS(&commandQueueOwnerShip));

			Ensures(commandQueueOwnerShip != nullptr);
		}

		void GraphicsDevice::CreateDescriptorHeap(DescriptorHeap& descriptorHeap) const noexcept
		{
			ComPtr<ID3D12DescriptorHeap>& ownership = descriptorHeap.GetDescriptorHeapOwnership();

			Expects(ownership == nullptr);

			mD3d12Device->CreateDescriptorHeap(&descriptorHeap.GetDescription(), IID_PPV_ARGS(&ownership));

			Ensures(ownership != nullptr);
		}

		void GraphicsDevice::CreateRenderTargetView(RenderTargetDescriptorHeap& renderTargetHeap, gsl::index heapIndex) const noexcept
		{
			std::vector<GpuResource>& gpuResources = renderTargetHeap.GetGpuResources();
			Expects(heapIndex < gpuResources.size());

			const ComPtr<ID3D12Resource>& renderTargetResourceOwner = gsl::at(gpuResources, heapIndex).GetD3d12ResourceOwner();
			
			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = renderTargetHeap.GetHeapStartHandle();
			descriptorHandle.ptr += GetRenderTargetHeapHandleIncrementSize() * heapIndex;

			mD3d12Device->CreateRenderTargetView(renderTargetResourceOwner.Get(), nullptr, descriptorHandle);
		}

		void GraphicsDevice::CreateRootSignature(RootSignature& rootSignature) const noexcept
		{
			ComPtr<ID3D12RootSignature>& rootSignatureOwner = rootSignature.GetD3d12RootSignatureOwner();
			gsl::not_null<ID3DBlob*> serializedRootSignature = rootSignature.GetSerializedRootSignature();
			mD3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer()
				, serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignatureOwner));

			Ensures(rootSignatureOwner != nullptr);
		}
	}
}