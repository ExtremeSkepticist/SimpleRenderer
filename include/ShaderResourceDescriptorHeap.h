#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		
		class ShaderResourceDescriptorHeap : public DescriptorHeap
		{
		public:
			ShaderResourceDescriptorHeap() noexcept;
			~ShaderResourceDescriptorHeap() = default;

			void BindResource(GraphicsDevice& graphicsDevice) override final
			{
			}
		};
	}
}