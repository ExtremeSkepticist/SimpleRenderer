#pragma once

#include <d3d12.h>
#include <DirectXTex.h>

#include "Common.h"
#include "GpuResource.h"


namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		// Texture = Gpu Resource for texture + Cpu Resource for texture
		class Texture
		{
		public:
			enum class TextureType
			{
				Albedo,
				MetalRoughness,
				Emissive,
				Normal,
				AmbientOcclusion,
				Num
			};

			Texture(gsl::not_null<gsl::cwzstring> textureName, unique_ptr<DirectX::ScratchImage> rawImage
			, const GraphicsDevice& graphicsDevice) noexcept;
			~Texture() = default;

			inline GpuResource& GetDefaultHeapResource() noexcept
			{
				return mDefaultHeap;
			}
			
			inline GpuResource& GetUploadHeapResource() noexcept
			{
				return mUploadHeap;
			}

			inline D3D12_SUBRESOURCE_DATA GetSubResourceData() const noexcept
			{
				return mSubResourceData;
			}

			inline DirectX::TexMetadata GetMetaData() const noexcept
			{
				return mRawImage->GetMetadata();
			}

		private:
			GpuResource mDefaultHeap;
			GpuResource mUploadHeap;
			uint8_t* mMappedMemoryForUploadHeap = nullptr;
			unique_ptr<DirectX::ScratchImage> mRawImage = nullptr;
			D3D12_SUBRESOURCE_DATA mSubResourceData = { 0, };
		};
	}
}