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
			RenderTargetDescriptorHeap(const uint32_t numDescriptors, SwapChain& swapChain) noexcept;

			RenderTargetDescriptorHeap() = delete;
			RenderTargetDescriptorHeap(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap& operator=(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap(RenderTargetDescriptorHeap&&) = delete;
			RenderTargetDescriptorHeap& operator=(RenderTargetDescriptorHeap&&) = delete;
			~RenderTargetDescriptorHeap() = default;

			void BindResource(GraphicsDevice& graphicsDevice) override final
			{
			}
		};
	}
}