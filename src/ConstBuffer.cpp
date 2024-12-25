#include "ConstBuffer.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		ConstBuffer::ConstBuffer(gsl::not_null<gsl::cwzstring> name, const CpuConstBufferData& cpuConstBufferData
			, const GraphicsDevice& graphicsDevice) noexcept
			: mMappedMemoryForCpuConstBufferData(nullptr)
			, mCpuConstBufferData({ cpuConstBufferData })
			, mGpuResource(name, GetCpuConstBufferDataSize(), ResourceHeapType::Upload, ResourceState::GenericRead)
		{
			graphicsDevice.CreateGpuResource(mGpuResource);

			// Map and initialize the constant buffer. We don't unmap this until the
			// app closes. Keeping things mapped for the lifetime of the resource is okay.
			const CD3DX12_RANGE constBufferReadRange(0, 0);
			const ComPtr<ID3D12Resource>& d3d12ConstBufferResourceOwner = mGpuResource.GetD3d12ResourceOwner();
			void** mappedAddressForConstBufferData = reinterpret_cast<void**>(&mMappedMemoryForCpuConstBufferData);

			d3d12ConstBufferResourceOwner->Map(0, &constBufferReadRange, mappedAddressForConstBufferData);
			memcpy(mMappedMemoryForCpuConstBufferData, &mCpuConstBufferData, GetCpuConstBufferDataSize());

			Ensures(mMappedMemoryForCpuConstBufferData != nullptr);
		}
	}
}