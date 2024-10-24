#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class ConstBuffer;
		class DescriptorHeap;
		class Fence;
		class GraphicsCommandList;
		class GraphicsCommandQueue;
		class GraphicsInfrastructure;
		class GraphicsPipelineState;
		class RenderTargetDescriptorHeap;
		class RootSignature;

		class GraphicsDevice
		{
		public:
			GraphicsDevice(const GraphicsInfrastructure& graphicsInfrastructure) noexcept;

			GraphicsDevice() = delete;
			GraphicsDevice(const GraphicsDevice&) = delete;
			GraphicsDevice(GraphicsDevice&&) = delete;
			GraphicsDevice& operator=(const GraphicsDevice&&) = delete;
			GraphicsDevice& operator=(GraphicsDevice&&) = delete;
			~GraphicsDevice() = default;

			void CreateCommandQueue(GraphicsCommandQueue& commandQueue) const noexcept;
			void CreateDescriptorHeap(DescriptorHeap& descriptorHeap) const noexcept;
			void CreateRenderTargetView(RenderTargetDescriptorHeap& renderTargetHeap, gsl::index heapIndex) const noexcept;
			void CreateRootSignature(RootSignature& rootSignature) const noexcept;
			void CreateCommandList(GraphicsCommandList& graphicsCommandList) const noexcept;
			void CreateGraphicsPipelineState(GraphicsPipelineState& graphicsPipelineState) const noexcept;
			void CreateFence(Fence& fence) const noexcept;
			void CreateConstBuffer(ConstBuffer& constBuffer) const noexcept;

			inline uint32_t GetRenderTargetHeapHandleIncrementSize() const noexcept
			{
				return mD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			}

			// TODO: to be removed; D3D12Device should not be exposed
			inline ID3D12Device* const GetD3d12Device() const noexcept
			{
				return mD3d12Device.Get();
			}

			
		private:
			ComPtr<ID3D12Device> mD3d12Device = nullptr;
		};
	}
}