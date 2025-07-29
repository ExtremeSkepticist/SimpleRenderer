#include <d3dx12.h>


#include "GpuResource.h"
#include "ShaderResourceDescriptorHeap.h" // Definition order for unique_ptr

namespace sr
{
	namespace rendering
	{
		ShaderResourceDescriptorHeap::ShaderResourceDescriptorHeap() noexcept
			: DescriptorHeap()
		{
			D3D12_DESCRIPTOR_HEAP_DESC& heapDescription = DescriptorHeap::GetDescription();
			heapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			heapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		}

		DescriptorHandle ShaderResourceDescriptorHeap::GetHeapStartHandle() const noexcept
		{
			DescriptorHandle handle = {
				GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart()
				, GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
			};

			return handle;
		}

		//void ShaderResourceDescriptorHeap::BindResource(GraphicsDevice& graphicsDevice)
		//{

		//}
	}
}