#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace platform
	{
		class PlatformWindow;
	}

	namespace rendering
	{
		class ConstBuffer;
		class CommandList;
		class CommandQueue;
		class DescriptorHeap;
		class Fence;
		class GpuResource;
		class GraphicsPipelineState;
		class RenderTargetDescriptorHeap;
		class RootSignature;
		class ShaderResourceDescriptorHeap;
		class Texture;
		struct DescriptorHandle;
		class SwapChain;

		class GraphicsDevice
		{
		public:
			GraphicsDevice() noexcept;

			GraphicsDevice(const GraphicsDevice&) = delete;
			GraphicsDevice(GraphicsDevice&&) = delete;
			GraphicsDevice& operator=(const GraphicsDevice&&) = delete;
			GraphicsDevice& operator=(GraphicsDevice&&) = delete;
			~GraphicsDevice() = default;

			void CreateCommandQueue(CommandQueue& commandQueue) const noexcept;
			void CreateSwapChain(SwapChain& swapChain, sr::platform::PlatformWindow& platformWindow, CommandQueue& commandQueue) const noexcept;
			void CreateDescriptorHeap(DescriptorHeap& descriptorHeap) const noexcept;
			void CreateRenderTargetView(const GpuResource& resource, DescriptorHandle& descriptorHandle) const noexcept;
			void CreateRootSignature(RootSignature& rootSignature) const noexcept;
			void CreateCommandList(CommandList& commandList) const noexcept;
			void CreateGraphicsPipelineState(GraphicsPipelineState& graphicsPipelineState) const noexcept;
			void CreateFence(Fence& fence) const noexcept;
			void CreateConstBufferView(ConstBuffer& constBuffer, DescriptorHandle& descriptorHandle) const noexcept;
			void CreateShaderResourceView(Texture& texture, DescriptorHandle& descriptorHandle) const noexcept;

			void CreateGpuResource(GpuResource& gpuResource) const noexcept;

			inline uint32_t GetRenderTargetHeapHandleIncrementSize() const noexcept
			{
				return mD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			}

			inline uint32_t GetShaderResourceDescriptorHeapHandleIncrementSize() const noexcept
			{
				return mD3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			}

			// TODO: to be removed; D3D12Device should not be exposed
			inline gsl::not_null<ID3D12Device*> const GetD3d12Device() const noexcept
			{
				return mD3d12Device.Get();
			}

		private:
			ComPtr<ID3D12Device> mD3d12Device = nullptr;
		};
	}
}