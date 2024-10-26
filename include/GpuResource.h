#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GpuResource
		{
		public:
			GpuResource() noexcept;

			inline ComPtr<ID3D12Resource>& GetD3d12ResourceOwner() noexcept
			{
				return mD3d12Resource;
			}

			inline ID3D12Resource* GetD3d12Resource() const noexcept
			{
				return mD3d12Resource.Get();
			}

		private:
			ComPtr<ID3D12Resource> mD3d12Resource = nullptr;
		};
	}
}