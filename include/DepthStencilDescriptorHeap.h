
#include <d3d12.h>

#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		class SwapChain;

		class DepthStencilDescriptorHeap : public DescriptorHeap
		{
		public:
			DepthStencilDescriptorHeap() noexcept;
			~DepthStencilDescriptorHeap() = default;

			DescriptorHandle GetHeapStartHandle() const noexcept override;

			void BindResource(GraphicsDevice& graphicsDevice) override final
			{
			}
		
		private:
		};
	}
}