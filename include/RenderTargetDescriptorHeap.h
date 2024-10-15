#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		class SwapChain;

		class RenderTargetDescriptorHeap : public DescriptorHeap
		{
		public:
			RenderTargetDescriptorHeap(const uint32_t numDescriptors, SwapChain& swapChain, const GraphicsDevice& graphicsDevice) noexcept;

			RenderTargetDescriptorHeap() = delete;
			RenderTargetDescriptorHeap(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap& operator=(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap(RenderTargetDescriptorHeap&&) = delete;
			RenderTargetDescriptorHeap& operator=(RenderTargetDescriptorHeap&&) = delete;
			~RenderTargetDescriptorHeap() = default;

			inline D3D12_CPU_DESCRIPTOR_HANDLE GetHeapStartHandle() const noexcept
			{
				return GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
			}


			void BindResource(GraphicsDevice& graphicsDevice) override final
			{
			}
		};
	}
}