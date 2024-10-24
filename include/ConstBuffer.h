#pragma once

// TODO: get rid of d3dx12: circular dependency or ambiguous definition issue
#include <d3dx12.h>
#include <DirectXMath.h>

#include "Common.h"
#include "GpuResource.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		typedef struct
		{
			DirectX::XMMATRIX mWorldMatrix;
			DirectX::XMMATRIX mViewMatrix;
			DirectX::XMMATRIX mProjectionMatrix;
			DirectX::XMMATRIX mUnused;
		} MappableConstBuffer;

		class ConstBuffer
		{	
		public:
			ConstBuffer(const MappableConstBuffer& mappableConstBuffer, const GraphicsDevice& graphicsDevice) noexcept;

			inline GpuResource& GetGpuResource() noexcept
			{
				return mGpuResource;
			}

			inline constexpr size_t GetMappableConstBufferSize() const noexcept
			{
				return sizeof(MappableConstBuffer);
			}

		private:
			uint8_t* mMappedCpuMemoryForConstBuffer = nullptr;
			MappableConstBuffer mMappableConstBuffer = {};
			GpuResource mGpuResource = GpuResource();
		};
	}
}