#include <d3dx12.h>

#include "GpuResource.h"
#include "GraphicsDevice.h"
#include "ShaderResourceDescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		ShaderResourceDescriptorHeap::ShaderResourceDescriptorHeap(const uint32_t numDescriptors, const GraphicsDevice& graphicsDevice) noexcept
			: DescriptorHeap(numDescriptors)
		{
			D3D12_DESCRIPTOR_HEAP_DESC& heapDescription = DescriptorHeap::GetDescription();
			heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

			graphicsDevice.CreateDescriptorHeap(*this);

			const ComPtr<ID3D12DescriptorHeap>& descriptorHeapOwner = GetDescriptorHeapOwnership();
			assert(descriptorHeapOwner != nullptr);
		}

		void ShaderResourceDescriptorHeap::BindResource(GraphicsDevice& graphicsDevice)
		{

		}
	}
}