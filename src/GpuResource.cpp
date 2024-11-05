#include <GraphicsDevice.h>

#include "GpuResource.h"

namespace sr
{
	namespace rendering
	{
		GpuResource::GpuResource(const ResourceType& resourceType, const ResourceHeapType& resourceHeapType, const GraphicsDevice& graphicsDevice) noexcept
			: mResourceType(resourceType)
			, mResourceHeapType(resourceHeapType)
			, mResourceDesc({})
			, mResourceHeapProps({})
			, mD3d12Resource(nullptr)
		{

		}
	}
}