#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class Fence
		{
		public:
			Fence() noexcept;
			~Fence() = default;

			inline ID3D12Fence* GetD3d12Fence() const noexcept
			{
				return mD3d12Fence.Get();
			}

			inline ComPtr<ID3D12Fence>& GetD3d12FenceOwner() noexcept
			{
				return mD3d12Fence;
			}

			inline uint64_t GetCurrentFenceValue() const noexcept
			{
				return mD3d12Fence->GetCompletedValue();
			}

			inline HANDLE& GetFenceEventOwner() noexcept
			{
				return mFenceEvent;
			}

		private:
			ComPtr<ID3D12Fence> mD3d12Fence = nullptr;
			HANDLE mFenceEvent = nullptr;
		};
	}
}