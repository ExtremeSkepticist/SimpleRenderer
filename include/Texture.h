#pragma once

#include "Common.h"
#include "GpuResource.h"

#include <d3d12.h>
#include <DirectXTex.h>

namespace sr
{
	namespace rendering
	{
		// Texture = Gpu Resource for texture + Cpu Resource for texture
		class Texture
		{
		public:
			Texture(const gsl::not_null<DirectX::Image*> image) noexcept;
			~Texture() = default;

		private:
			DirectX::TexMetadata mTextureMetadata = { 0, };
			DirectX::ScratchImage mScratchImage = {};
			GpuResource mDefaultHeap;
			GpuResource mUploadHeap;
			uint8_t* mMappedMemoryForUploadHeap = nullptr;
		};
	}
}