#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		
		class ShaderResourceDescriptorHeap : public DescriptorHeap
		{
		public:
			ShaderResourceDescriptorHeap(const uint32_t numDescriptors, const GraphicsDevice& graphicsDevice) noexcept;
			~ShaderResourceDescriptorHeap() = default;

			inline D3D12_GPU_DESCRIPTOR_HANDLE GetHeapStartHandle() const noexcept
			{
				return GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
			}


			void BindResource(GraphicsDevice& graphicsDevice) override final;
		};
	}
}