#include "ConstBuffer.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		ConstBuffer::ConstBuffer(const MappableConstBuffer& mappableConstBuffer, const GraphicsDevice& graphicsDevice) noexcept
			: mMappedCpuMemoryForConstBuffer(nullptr)
			, mMappableConstBuffer({mappableConstBuffer})
		{
			graphicsDevice.CreateConstBuffer(*this);

			// Map and initialize the constant buffer. We don't unmap this until the
			// app closes. Keeping things mapped for the lifetime of the resource is okay.
			const CD3DX12_RANGE constBufferReadRange(0, 0);
			ComPtr<ID3D12Resource>& d3d12ConstBufferResourceOwner = mGpuResource.GetD3d12ResourceOwner();
			d3d12ConstBufferResourceOwner->Map(0, &constBufferReadRange, reinterpret_cast<void**>(&mMappedCpuMemoryForConstBuffer));
			memcpy(mMappedCpuMemoryForConstBuffer, &mMappableConstBuffer, GetMappableConstBufferSize());

			Ensures(mMappedCpuMemoryForConstBuffer != nullptr);
		}
	}
}