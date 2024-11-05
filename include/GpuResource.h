#pragma once

#include <d3d12.h>
#include <d3dx12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		enum class ResourceType
		{
			None
			, Texture2D
			, Buffer
			, Num
		};
		enum class ResourceHeapType
		{
			None
			, Default
			, Upload
			, Num
		};

		class GpuResource
		{
		public:
			GpuResource(const ResourceType& resourceType,const ResourceHeapType& resourceHeapType, const GraphicsDevice& graphicsDevice) noexcept;

			inline ComPtr<ID3D12Resource>& GetD3d12ResourceOwner() noexcept
			{
				return mD3d12Resource;
			}

			inline ID3D12Resource* GetD3d12Resource() const noexcept
			{
				return mD3d12Resource.Get();
			}

		private:
			ResourceType mResourceType = ResourceType::None;
			ResourceHeapType mResourceHeapType = ResourceHeapType::None;
			CD3DX12_RESOURCE_DESC mResourceDesc = {};
			CD3DX12_HEAP_PROPERTIES mResourceHeapProps = {};
			ComPtr<ID3D12Resource> mD3d12Resource = nullptr;

		};
	}
}