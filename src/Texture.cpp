#include <d3dx12.h>

#include "GraphicsDevice.h"
#include "Texture.h"

namespace sr
{
	namespace rendering
	{
		// TODO: Load image outside and pass into this, then initialize GpuResource
		// Texture should be just a container for texture-related data
		// TODO: How to pass ownership to this?
		// 1. Sharing ownership with local ScratchIamge, using shared_ptr
		Texture::Texture(gsl::not_null<gsl::cwzstring> textureName, unique_ptr<DirectX::ScratchImage> rawImage, const GraphicsDevice& graphicsDevice) noexcept
			: mDefaultHeap(textureName, *rawImage->GetImage(0, 0, 0), ResourceHeapType::Default, ResourceState::CopyDestination, graphicsDevice)
			, mUploadHeap(textureName, GetRequiredIntermediateSize(mDefaultHeap.GetD3d12Resource(), 0, 1), ResourceHeapType::Upload, ResourceState::GenericRead, graphicsDevice)
			, mMappedMemoryForUploadHeap(nullptr)
			, mRawImage(std::move(rawImage))
			, mSubResourceData( { 0, })
		{
			const gsl::not_null<ID3D12Resource*> d3d12UploadHeapResource = mUploadHeap.GetD3d12Resource();
			const CD3DX12_RANGE uploadReadRange(0, 0);
			gsl::not_null<const DirectX::Image*> image = mRawImage->GetImage(0, 0, 0);
			d3d12UploadHeapResource->Map(0, &uploadReadRange, reinterpret_cast<void**>(&mMappedMemoryForUploadHeap));
			{
				memcpy(mMappedMemoryForUploadHeap, image->pixels, image->slicePitch);
			}
			d3d12UploadHeapResource->Unmap(0, nullptr);

			// Copy from upload to default heap
			mSubResourceData.pData = image->pixels;
			mSubResourceData.RowPitch = image->width * sizeof(uint32_t); // R8G8B8A8 format per pixel
			mSubResourceData.SlicePitch = image->height * mSubResourceData.RowPitch;
		}
	}
}