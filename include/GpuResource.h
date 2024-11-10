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
			, Common
			, GenericRead
			, CopyDestination
			, RenderTarget
			, Num
		};

		class GpuResource
		{
		public:
			GpuResource() = default;
			
			GpuResource(gsl::not_null<gsl::cwzstring> resourceName, const size_t bufferSize, const ResourceHeapType& resourceHeapType
				, const ResourceState& resourceState, const GraphicsDevice& graphicsDevice) noexcept;

			GpuResource(gsl::not_null<gsl::cwzstring> resourceName, const DirectX::Image& image, const ResourceHeapType& resourceHeapType
				, const ResourceState& resourceState, const GraphicsDevice& graphicsDevice) noexcept;

			inline gsl::not_null<gsl::cwzstring> GetResourceName() const noexcept
			{
				return mResourceName;
			}

			inline void SetResourceName(gsl::not_null<gsl::cwzstring> resourceName) noexcept
			{
				mResourceName = resourceName;
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

			inline void SetResourceState(const ResourceState resourceState) noexcept
			{
				mResourceState = resourceState;
			}

			inline CD3DX12_RESOURCE_DESC GetResourceDescription() const noexcept
			{
				return mResourceDesc;
			}

			inline void SetResourceDescription(const CD3DX12_RESOURCE_DESC resourceDesc) noexcept
			{
				mResourceDesc = resourceDesc;
			}

			inline CD3DX12_HEAP_PROPERTIES GetResourceHeapProperties() const noexcept
			{
				return mResourceHeapProps;
			}

			inline void SetResourceHeapProperties(const CD3DX12_HEAP_PROPERTIES heapProps) noexcept
			{
				mResourceHeapProps = heapProps;
			}

		private:
			gsl::not_null<gsl::cwzstring> mResourceName = L"";
			CD3DX12_RESOURCE_DESC mResourceDesc = {};
			ResourceState mResourceState = ResourceState::None;
			CD3DX12_HEAP_PROPERTIES mResourceHeapProps = {};
			ComPtr<ID3D12Resource> mD3d12Resource = nullptr;
		};
	}
}