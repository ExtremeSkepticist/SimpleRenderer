#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsInfrastructure;
		class GraphicsCommandQueue;

		class GraphicsDevice
		{
		public:
			GraphicsDevice(const GraphicsInfrastructure& graphicsInfrastructure) noexcept;

			GraphicsDevice() = delete;
			GraphicsDevice(const GraphicsDevice&) = delete;
			GraphicsDevice(GraphicsDevice&&) = delete;
			GraphicsDevice& operator=(const GraphicsDevice&&) = delete;
			GraphicsDevice& operator=(GraphicsDevice&&) = delete;
			~GraphicsDevice() = default;

			void CreateCommandQueue(GraphicsCommandQueue& commandQueue, const GraphicsInfrastructure& graphicsInfrastructure);

			// TODO: to be removed; D3D12Device should not be exposed
			inline ID3D12Device* GetDevice() const noexcept
			{
				return mD3d12Device.Get();
			}

		private:
			ComPtr<ID3D12Device> mD3d12Device = nullptr;
		};
	}
}