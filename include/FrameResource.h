#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		class GraphicsPipelineState;
		class RenderTargetDescriptorHeap;
		class RootSignature;
		class ShaderResourceDescriptorHeap;

		struct DescriptorHandle;

		class FrameResource
		{
		public:
			FrameResource(const GraphicsDevice& device);
			RenderTargetDescriptorHeap& GetRenderTargetDescriptorHeap();
			ShaderResourceDescriptorHeap& GetShaderResourceDescriptorHeap();

		private:
			void createDefaultRootSignature(const GraphicsDevice& device);
			void createDefaultPipelineState(const GraphicsDevice& device);

			unique_ptr<RenderTargetDescriptorHeap> mRenderTargetDescriptorHeap = nullptr;
			unique_ptr<RootSignature> mRootSignature = nullptr;
			unique_ptr<GraphicsPipelineState> mPipelineState = nullptr;
			unique_ptr<ShaderResourceDescriptorHeap> mShaderResourceDescriptorHeap = nullptr;
		};
	}
}
