#include "Fence.h"
#include "GraphicsDevice.h"

namespace sr
{
	namespace rendering
	{
		Fence::Fence() noexcept
			: mD3d12Fence(nullptr)
			, mFenceEvent(nullptr)
		{
			mFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		}
	}
}