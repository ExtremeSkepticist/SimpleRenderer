#include <GraphicsDevice.h>

#include "GpuResource.h"

namespace sr
{
	namespace rendering
	{
		GpuResource::GpuResource(gsl::not_null<gsl::wzstring> resourceName, const size_t bufferSize, const ResourceHeapType& resourceHeapType
			, const ResourceState& resourceState, const GraphicsDevice& graphicsDevice) noexcept
			: mResourceName(resourceName)
			, mResourceState(resourceState)
			, mResourceDesc({})
			, mResourceHeapProps({})
			, mD3d12Resource(nullptr)
		{
			Expects(bufferSize > 0);

			mResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

			switch (resourceHeapType)
			{
				case ResourceHeapType::Default:
					mResourceHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
					break;
				case ResourceHeapType::Upload:
					mResourceHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
					break;
				default:
					assert(false);
			}

			graphicsDevice.CreateGpuResource(*this);

			Ensures(mD3d12Resource != nullptr);
		}

		GpuResource::GpuResource(gsl::not_null<gsl::wzstring> resourceName, const DirectX::Image& image, const ResourceHeapType& resourceHeapType
			, const ResourceState& resourceState, const GraphicsDevice& graphicsDevice) noexcept
			: mResourceName(resourceName)
			, mResourceState(resourceState)
			, mResourceDesc({})
			, mResourceHeapProps({})
			, mD3d12Resource(nullptr)
		{
			mResourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(image.format, image.width, image.height);

			switch (resourceHeapType)
			{
			case ResourceHeapType::Default:
				mResourceHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
				break;
			case ResourceHeapType::Upload:
				mResourceHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
				break;
			default:
				assert(false);
			}

			graphicsDevice.CreateGpuResource(*this);

			Ensures(mD3d12Resource != nullptr);
		}

	}
}