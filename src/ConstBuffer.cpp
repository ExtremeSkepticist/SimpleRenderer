#include "ConstBuffer.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		ConstBuffer::ConstBuffer(gsl::not_null<gsl::cwzstring> name, const GraphicsDevice& graphicsDevice) noexcept
			: mMappedMemoryForCpuConstBufferData(nullptr)
			, mCpuConstBufferData({})
			, mGpuResource(name, GetCpuConstBufferDataSize(), ResourceHeapType::Upload, ResourceState::GenericRead)
		{
			graphicsDevice.CreateGpuResource(mGpuResource);
		}

		void ConstBuffer::Update(const CpuConstBufferData& cpuConstBufferData) noexcept
		{
			mCpuConstBufferData = cpuConstBufferData;
			const CD3DX12_RANGE constBufferReadRange(0, 0);
			const ComPtr<ID3D12Resource>& d3d12ConstBufferResourceOwner = mGpuResource.GetD3d12ResourceOwner();
			void** mappedAddressForConstBufferData = reinterpret_cast<void**>(&mMappedMemoryForCpuConstBufferData);

			d3d12ConstBufferResourceOwner->Map(0, &constBufferReadRange, mappedAddressForConstBufferData);
			memcpy(mMappedMemoryForCpuConstBufferData, &mCpuConstBufferData, GetCpuConstBufferDataSize());
			d3d12ConstBufferResourceOwner->Unmap(0, nullptr);
		}
	}
}