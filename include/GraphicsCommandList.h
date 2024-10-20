#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

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

			inline ComPtr<ID3D12CommandAllocator>& GetD3d12CommandAllocatorOwner() noexcept
			{
				return mD3d12CommandAllocator;
			}

			inline ComPtr<ID3D12GraphicsCommandList>& GetD3d12GraphicsCommandListOwner() noexcept
			{
				return mD3d12CommandList;
			}

		private:
			ComPtr<ID3D12CommandAllocator> mD3d12CommandAllocator = nullptr;
			ComPtr<ID3D12GraphicsCommandList> mD3d12CommandList = nullptr;
		};
	}
}