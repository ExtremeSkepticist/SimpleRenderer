#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		class DescriptorHeap
		{
		public:
			DescriptorHeap(uint32_t numDescriptors) noexcept;
			
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

			inline ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeapOwnership() noexcept
			{
				return mD3d12DescriptorHeap;
			}

		private:
			uint32_t mNumAllocatedDescriptors = 0;
			D3D12_DESCRIPTOR_HEAP_DESC mDescriptorHeapDescription = {};
			ComPtr<ID3D12DescriptorHeap> mD3d12DescriptorHeap = nullptr;
		};
	}
}