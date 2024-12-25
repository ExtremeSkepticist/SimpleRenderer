#pragma once

#include <d3d12.h>

#include "Common.h"
#include "DescriptorHandle.h"
#include "GpuResource.h"

namespace sr
{
	namespace rendering
	{
		constexpr static size_t defaultNumDescriptor = 128;

		class GraphicsDevice;
		
		// TODO: Separate GpuResource from DescriptorHeap
		class DescriptorHeap
		{
		public:
			DescriptorHeap() noexcept;

			DescriptorHeap(DescriptorHeap&&) = delete;
			DescriptorHeap& operator=(DescriptorHeap&&) = delete;
			DescriptorHeap(const DescriptorHeap&) = delete;
			DescriptorHeap& operator=(const DescriptorHeap&) = delete;
			~DescriptorHeap() = default;

			virtual void BindResource(GraphicsDevice& graphicsDevice) = 0;

			inline D3D12_DESCRIPTOR_HEAP_DESC& GetDescription() noexcept
			{
				return mDesc;
			}

			inline ID3D12DescriptorHeap* const GetDescriptorHeap() const noexcept
			{
				return mD3d12DescriptorHeap.Get();
			}

			inline ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeapOwnership() noexcept
			{
				return mD3d12DescriptorHeap;
			}


			virtual DescriptorHandle GetHeapStartHandle() const noexcept = 0;

			inline size_t GetCapacity() const noexcept
			{
				return mCapacity;
			}

			inline size_t GetSize() const noexcept
			{
				return mSize;
			}

		private:
			D3D12_DESCRIPTOR_HEAP_DESC mDesc = {};
			ComPtr<ID3D12DescriptorHeap> mD3d12DescriptorHeap = nullptr;
			size_t mCapacity = defaultNumDescriptor;
			size_t mSize = 0;
		};
	}
}