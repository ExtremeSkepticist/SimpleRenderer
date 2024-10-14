#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		class RenderTargetDescriptorHeap : public DescriptorHeap
		{
		public:
			RenderTargetDescriptorHeap(uint32_t numDescriptors) noexcept;

			RenderTargetDescriptorHeap() = delete;
			RenderTargetDescriptorHeap(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap& operator=(const RenderTargetDescriptorHeap&) = delete;
			RenderTargetDescriptorHeap(RenderTargetDescriptorHeap&&) = delete;
			RenderTargetDescriptorHeap& operator=(RenderTargetDescriptorHeap&&) = delete;
			~RenderTargetDescriptorHeap() = default;

			void BindResource(GraphicsDevice& graphicsDevice) override;
		};
	}
}