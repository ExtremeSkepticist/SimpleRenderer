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

		// Aligning with 256 bytes
		typedef struct CpuConstBufferData
		{
			DirectX::XMMATRIX mWorldMatrix;
			DirectX::XMMATRIX mViewMatrix;
			DirectX::XMMATRIX mProjectionMatrix;
			DirectX::XMMATRIX mLightPosition;
		} CpuConstBufferData;

		class ConstBuffer
		{	
		public:
			ConstBuffer(gsl::not_null<gsl::cwzstring> name, const CpuConstBufferData& cpuConstBufferData, const GraphicsDevice& graphicsDevice) noexcept;

			inline GpuResource& GetGpuResource() noexcept
			{
				return mGpuResource;
			}

			inline constexpr size_t GetCpuConstBufferDataSize() const noexcept
			{
				return sizeof(CpuConstBufferData);
			}

			void Update(const CpuConstBufferData& cpuConstBufferData);

		private:
			uint8_t* mMappedMemoryForCpuConstBufferData = nullptr;
			CpuConstBufferData mCpuConstBufferData = {};
			GpuResource mGpuResource;
		};
	}
}