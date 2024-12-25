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
			RenderTargetDescriptorHeap() noexcept;

			RenderTargetDescriptorHeap(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap& operator=(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap(RenderTargetDescriptorHeap&&) = delete;
			RenderTargetDescriptorHeap& operator=(RenderTargetDescriptorHeap&&) = delete;
			~RenderTargetDescriptorHeap() = default;

			DescriptorHandle GetHeapStartHandle() const noexcept override;

			void BindResource(GraphicsDevice& graphicsDevice) override final
			{
			}
		};
	}
}