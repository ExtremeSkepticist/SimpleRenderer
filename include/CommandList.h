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
		enum class CommandType;

		class CommandList
		{
		public:
			CommandList(const CommandType& type) noexcept;

			void ResetCommandList() noexcept;

			inline void FinishRecordingCommands() noexcept
			{
				Expects(mD3d12CommandList != nullptr);

				mD3d12CommandList->Close();
			}

			void RecordCommandCopyResource(GpuResource& destResource, const GpuResource& srcResource, const D3D12_SUBRESOURCE_DATA& subResourceData) noexcept;

			inline ComPtr<ID3D12CommandAllocator>& GetD3d12CommandAllocatorOwner() noexcept
			{
				return mD3d12CommandAllocator;
			}

			inline ComPtr<ID3D12GraphicsCommandList>& GetD3d12CommandListOwner() noexcept
			{
				return mD3d12CommandList;
			}

			inline ID3D12GraphicsCommandList* GetCommandList() const noexcept
			{
				return mD3d12CommandList.Get();
			}
			 
			inline CommandType GetCommandListType() const noexcept
			{
				return mType;
			}

			void SetPipelineState(const GraphicsPipelineState& graphicsPipelineState) const noexcept;

			void SetGraphicsRootSignature(const RootSignature& rootSignature) const noexcept;

		private:
			CommandType mType;
			ComPtr<ID3D12CommandAllocator> mD3d12CommandAllocator = nullptr;
			ComPtr<ID3D12GraphicsCommandList> mD3d12CommandList = nullptr;
		};
	}
}