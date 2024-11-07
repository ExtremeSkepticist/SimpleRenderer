#include <d3dx12.h>

#include "Texture.h"

namespace sr
{
	namespace rendering
	{
		// TODO: Load image outside and pass into this, then initialize GpuResource
		// Texture should be just a container for texture-related data
		// TODO: How to pass ownership to this?
		// 1. Sharing ownership with local ScratchIamge, using shared_ptr
		Texture::Texture(const gsl::not_null<DirectX::Image*>) noexcept
			: mTextureMetadata({ 0, })
			, mScratchImage({})
			, mDefaultHeap()
			, mUploadHeap()
			, mMappedMemoryForUploadHeap(nullptr)
		{
			
			// TODO
			// 1. Put resrouce desc & heap props into GpuResource
			// 2. Pass GraphicsDevice into GpuResource constructor
			// 3. Call GraphicsDevice::CreateGpuResource() to allocate heap
			/*CD3DX12_RESOURCE_DESC defaultHeapDesc = CD3DX12_RESOURCE_DESC::Tex2D(image->format, image->width, image->height);
			CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
			g_pDevice->CreateCommittedResource(
				&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &defaultHeapDesc
				, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&g_pAlbedoTexture));
			g_pAlbedoTexture->SetName(L"Albedo texture - default");*/


		}
	}
}