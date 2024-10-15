#pragma once

#include <d3d12.h>

#include "Common.h"
#include "GpuResource.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		
		class DescriptorHeap
		{
		public:
			DescriptorHeap(const uint32_t numDescriptors) noexcept;
			
			DescriptorHeap() = delete;
			DescriptorHeap(DescriptorHeap&&) = delete;
			DescriptorHeap& operator=(DescriptorHeap&&) = delete;
			DescriptorHeap(const DescriptorHeap&) = delete;
			DescriptorHeap& operator=(const DescriptorHeap&) = delete;
			~DescriptorHeap() = default;

			virtual void BindResource(GraphicsDevice& graphicsDevice) = 0;

			inline D3D12_DESCRIPTOR_HEAP_DESC& GetDescription() noexcept
			{
				return mDescriptorHeapDescription;
			}

			inline ID3D12DescriptorHeap* const GetDescriptorHeap() const noexcept
			{
				return mD3d12DescriptorHeap.Get();
			}

			inline ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeapOwnership() noexcept
			{
				return mD3d12DescriptorHeap;
			}

			inline std::vector<GpuResource>& GetGpuResources() noexcept
			{
				return mGpuResources;
			}

		private:
			D3D12_DESCRIPTOR_HEAP_DESC mDescriptorHeapDescription = {};
			ComPtr<ID3D12DescriptorHeap> mD3d12DescriptorHeap = nullptr;
			std::vector<GpuResource> mGpuResources = {};
		};
	}
}