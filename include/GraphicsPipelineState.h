#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		class InputElementLayout;
		class RootSignature;
		class Shader;

		class GraphicsPipelineState
		{
		public:
			GraphicsPipelineState(const RootSignature& rootSignature, InputElementLayout& inputElementLayout
				, const Shader& vertexShader, const Shader& pixelShader, const GraphicsDevice& graphicsDevice) noexcept;

			inline D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetGraphicsPipelineStateDescriptionOwner() noexcept
			{
				return mD3d12GraphicsPipelineStateDescription;
			}

			inline ComPtr<ID3D12PipelineState>& GetPipelineStateOwner() noexcept
			{
				return mD3d12PipelineState;
			}

		private:
			D3D12_GRAPHICS_PIPELINE_STATE_DESC mD3d12GraphicsPipelineStateDescription = {};
			ComPtr<ID3D12PipelineState> mD3d12PipelineState = nullptr;
		};
	}
}