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

#pragma pack(push, 256)
		// Aligning with 256 bytes
		typedef struct CpuConstBufferData
		{
			DirectX::XMMATRIX mWorldMatrix;
			DirectX::XMMATRIX mViewMatrix;
			DirectX::XMMATRIX mProjectionMatrix;
		} CpuConstBufferData;
#pragma pack(pop)

		class ConstBuffer
		{	
		public:
			ConstBuffer(gsl::not_null<gsl::cwzstring> name, const GraphicsDevice& graphicsDevice) noexcept;

			inline GpuResource& GetGpuResource() noexcept
			{
				return mGpuResource;
			}

			inline constexpr size_t GetCpuConstBufferDataSize() const noexcept
			{
				return sizeof(CpuConstBufferData);
			}

			void Update(const CpuConstBufferData& cpuConstBufferData) noexcept;

		private:
			uint8_t* mMappedMemoryForCpuConstBufferData = nullptr;
			CpuConstBufferData mCpuConstBufferData = {};
			GpuResource mGpuResource;
		};
	}
}