#include "GraphicsCommandList.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		GraphicsCommandList::GraphicsCommandList(const GraphicsDevice& graphicsDevice) noexcept
			: mD3d12CommandAllocator(nullptr)
			, mD3d12CommandList(nullptr)
		{
			graphicsDevice.CreateCommandList(*this);
		}
	}
}