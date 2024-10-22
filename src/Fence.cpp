#include "Fence.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		Fence::Fence(const GraphicsDevice& graphicsDevice) noexcept
			: mD3d12Fence(nullptr)
			, mFenceEvent(nullptr)
		{
			graphicsDevice.CreateFence(*this);
			mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		}
	}
}