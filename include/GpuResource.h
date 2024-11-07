#pragma once

#include <DirectXTex.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		enum class ResourceHeapType
		{
			None
			, Default
			, Upload
			, Num
		};

		enum class ResourceState
		{
			None
			, GenericRead
			, CopyDestination
			, Num
		};

		class GpuResource
		{
		public:
			GpuResource(gsl::not_null<gsl::wzstring> resourceName, const size_t bufferSize, const ResourceHeapType& resourceHeapType
				, const ResourceState& resourceState, const GraphicsDevice& graphicsDevice) noexcept;

			GpuResource(gsl::not_null<gsl::wzstring> resourceName, const DirectX::Image& image, const ResourceHeapType& resourceHeapType
				, const ResourceState& resourceState, const GraphicsDevice& graphicsDevice) noexcept;

			inline gsl::not_null<gsl::wzstring> GetResourceName() const noexcept
			{
				return mResourceName;
			}

			inline ComPtr<ID3D12Resource>& GetD3d12ResourceOwner() noexcept
			{
				return mD3d12Resource;
			}

			inline ID3D12Resource* GetD3d12Resource() const noexcept
			{
				return mD3d12Resource.Get();
			}

			inline ResourceState GetResourceState() const noexcept
			{
				return mResourceState;
			}

			inline CD3DX12_RESOURCE_DESC GetResourceDescription() const noexcept
			{
				return mResourceDesc;
			}

			inline CD3DX12_HEAP_PROPERTIES GetResourceHeapProperties() const noexcept
			{
				return mResourceHeapProps;
			}


		private:
			gsl::not_null<gsl::wzstring> mResourceName;
			CD3DX12_RESOURCE_DESC mResourceDesc = {};
			ResourceState mResourceState = ResourceState::None;
			CD3DX12_HEAP_PROPERTIES mResourceHeapProps = {};
			ComPtr<ID3D12Resource> mD3d12Resource = nullptr;
		};
	}
}