#include "ConstBuffer.h"
#include "DescriptorHeap.h"
#include "Fence.h"
#include "GpuResource.h"
#include "GraphicsCommandList.h"
#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"
#include "GraphicsInfrastructure.h"
#include "GraphicsPipelineState.h"
#include "RenderTargetDescriptorHeap.h"
#include "RootSignature.h"
#include "ShaderResourceDescriptorHeap.h"

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

		void GraphicsDevice::CreateCommandQueue(GraphicsCommandQueue& commandQueue) const noexcept
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
			
			D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle = renderTargetHeap.GetCpuHeapStartHandle();
			descriptorHandle.ptr += GetRenderTargetHeapHandleIncrementSize() * heapIndex;

			mD3d12Device->CreateRenderTargetView(renderTargetResourceOwner.Get(), nullptr, descriptorHandle);
		}

		void GraphicsDevice::CreateRootSignature(RootSignature& rootSignature) const noexcept
		{
			ComPtr<ID3D12RootSignature>& rootSignatureOwner = rootSignature.GetD3d12RootSignatureOwner();
			const gsl::not_null<ID3DBlob*> serializedRootSignature = rootSignature.GetSerializedRootSignature();
			mD3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer()
				, serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&rootSignatureOwner));

			Ensures(rootSignatureOwner != nullptr);
		}

		void GraphicsDevice::CreateCommandList(GraphicsCommandList& graphicsCommandList) const noexcept
		{
			ComPtr<ID3D12CommandAllocator>& d3d12CommandAllocatorOwner = graphicsCommandList.GetD3d12CommandAllocatorOwner();
			Expects(d3d12CommandAllocatorOwner == nullptr);

			constexpr D3D12_COMMAND_LIST_TYPE d3d12GraphicsCommandListType = D3D12_COMMAND_LIST_TYPE_DIRECT;
			mD3d12Device->CreateCommandAllocator(d3d12GraphicsCommandListType, IID_PPV_ARGS(&d3d12CommandAllocatorOwner));
			assert(d3d12CommandAllocatorOwner != nullptr);

			ComPtr<ID3D12GraphicsCommandList>& d3d12CommandListOwner = graphicsCommandList.GetD3d12GraphicsCommandListOwner();
			Expects(d3d12CommandListOwner == nullptr);
			
			constexpr uint32_t nodeMask = 0;
			ID3D12PipelineState* initialPipelineState = nullptr;
			mD3d12Device->CreateCommandList(nodeMask, d3d12GraphicsCommandListType
				, d3d12CommandAllocatorOwner.Get(), initialPipelineState, IID_PPV_ARGS(&d3d12CommandListOwner));

			Ensures(d3d12CommandListOwner != nullptr);

			d3d12CommandListOwner->Close();
		}

		void GraphicsDevice::CreateGraphicsPipelineState(GraphicsPipelineState& graphicsPipelineState) const noexcept
		{
			ComPtr<ID3D12PipelineState>& d3d12PipelineStateOwner = graphicsPipelineState.GetPipelineStateOwner();
			Expects(d3d12PipelineStateOwner == nullptr);

			const D3D12_GRAPHICS_PIPELINE_STATE_DESC& d3d12GraphicsPipelineStateDescriptionOwner 
				= graphicsPipelineState.GetGraphicsPipelineStateDescriptionOwner();
			
			mD3d12Device->CreateGraphicsPipelineState(&d3d12GraphicsPipelineStateDescriptionOwner, IID_PPV_ARGS(&d3d12PipelineStateOwner));
			Ensures(d3d12PipelineStateOwner != nullptr);
		}

		void GraphicsDevice::CreateFence(Fence& fence) const noexcept
		{
			ComPtr<ID3D12Fence>& d3d12FenceOwner = fence.GetD3d12FenceOwner();

			Expects(d3d12FenceOwner == nullptr);

			mD3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&d3d12FenceOwner));
		
			Ensures(d3d12FenceOwner != nullptr);
		}

		void GraphicsDevice::CreateConstBuffer(ConstBuffer& constBuffer) const noexcept
		{
			ComPtr<ID3D12Resource>& d3d12ConstBufferResourceOwner = constBuffer.GetGpuResource().GetD3d12ResourceOwner();
			Expects(d3d12ConstBufferResourceOwner == nullptr);
			
			const size_t mappableConstBufferSize = constBuffer.GetCpuConstBufferDataSize();
			const CD3DX12_HEAP_PROPERTIES d3dx12HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
			const CD3DX12_RESOURCE_DESC d3dx12ResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mappableConstBufferSize);
			mD3d12Device->CreateCommittedResource(
				&d3dx12HeapProperties
				, D3D12_HEAP_FLAG_NONE
				, &d3dx12ResourceDesc
				, D3D12_RESOURCE_STATE_GENERIC_READ
				, nullptr
				, IID_PPV_ARGS(&d3d12ConstBufferResourceOwner)
			);

			Ensures(d3d12ConstBufferResourceOwner != nullptr);

			d3d12ConstBufferResourceOwner->SetName(L"Const buffer resource");
		}

		void GraphicsDevice::CreateConstBufferView(ConstBuffer& constBuffer, ShaderResourceDescriptorHeap& shaderResourceDescriptorHeap
			, const gsl::index heapIndex) const noexcept
		{
			ID3D12Resource* const d3d12ResourceForConstBuffer = constBuffer.GetGpuResource().GetD3d12Resource();
			D3D12_CONSTANT_BUFFER_VIEW_DESC constBufferDesc = { 0, };
			constBufferDesc.BufferLocation = d3d12ResourceForConstBuffer->GetGPUVirtualAddress();
			constBufferDesc.SizeInBytes = constBuffer.GetCpuConstBufferDataSize();

			D3D12_CPU_DESCRIPTOR_HANDLE d3d12DescriptorHandle = shaderResourceDescriptorHeap.GetCpuHeapStartHandle();
			d3d12DescriptorHandle.ptr += GetShaderResourceDescriptorHeapHandleIncrementSize() * heapIndex;

			mD3d12Device->CreateConstantBufferView(&constBufferDesc, d3d12DescriptorHandle);
		}

		void GraphicsDevice::CreateGpuResource(GpuResource& gpuResource) const noexcept
		{
			const CD3DX12_RESOURCE_DESC resourceDesc = gpuResource.GetResourceDescription();
			const CD3DX12_HEAP_PROPERTIES resourceHeapProps = gpuResource.GetResourceHeapProperties();
			const ResourceState resourceState = gpuResource.GetResourceState();

			D3D12_RESOURCE_STATES d3d12ResourceState;
			switch (resourceState)
			{
			case ResourceState::GenericRead:
				d3d12ResourceState = D3D12_RESOURCE_STATE_GENERIC_READ;
				break;
			case ResourceState::CopyDestination:
				d3d12ResourceState = D3D12_RESOURCE_STATE_COPY_DEST;
				break;
			}

			ComPtr<ID3D12Resource>& d3d12ResourceOwner = gpuResource.GetD3d12ResourceOwner();
			mD3d12Device->CreateCommittedResource(
				&resourceHeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc
				, d3d12ResourceState, nullptr, IID_PPV_ARGS(&d3d12ResourceOwner));

			assert(d3d12ResourceOwner != nullptr);

			d3d12ResourceOwner->SetName(gpuResource.GetResourceName());
		}


	}
}