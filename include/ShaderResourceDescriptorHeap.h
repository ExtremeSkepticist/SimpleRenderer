#pragma once

#include "DescriptorHeap.h"

namespace sr
{
	namespace rendering
	{
		class ShaderResourceDescriptorHeap : public DescriptorHeap
		{
		public:
			ShaderResourceDescriptorHeap() noexcept;
			~ShaderResourceDescriptorHeap() = default;

			DescriptorHandle GetHeapStartHandle() const noexcept override;

			void BindResource(GraphicsDevice& graphicsDevice) override final
			{

			}
		};
	}
}