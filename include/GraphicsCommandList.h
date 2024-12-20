#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		class GraphicsResource;
		class GraphicsPipelineState;
		class RootSignature;
		class GpuResource;

		class GraphicsCommandList
		{
		public:
			enum class CommandListType
			{
				Direct
				, Copy
				, Compute
				, Num
			};

			GraphicsCommandList(const GraphicsDevice& graphicsDevice) noexcept;

			void ResetCommandList() noexcept;

			inline void FinishRecordingCommands() noexcept
			{
				mD3d12CommandList->Close();
			}

			void RecordCommandCopyResource(GpuResource& destResource, const GpuResource& srcResource, const D3D12_SUBRESOURCE_DATA& subResourceData) noexcept;

			inline ComPtr<ID3D12CommandAllocator>& GetD3d12CommandAllocatorOwner() noexcept
			{
				return mD3d12CommandAllocator;
			}

			inline ComPtr<ID3D12GraphicsCommandList>& GetD3d12GraphicsCommandListOwner() noexcept
			{
				return mD3d12CommandList;
			}

			inline ID3D12GraphicsCommandList* GetGraphicsCommandList() const noexcept
			{
				return mD3d12CommandList.Get();
			}

			void SetPipelineState(GraphicsPipelineState& graphicsPipelineState) const noexcept;

			void SetGraphicsRootSignature(RootSignature& rootSignature) const noexcept;

		private:
			ComPtr<ID3D12CommandAllocator> mD3d12CommandAllocator = nullptr;
			ComPtr<ID3D12GraphicsCommandList> mD3d12CommandList = nullptr;
		};
	}
}