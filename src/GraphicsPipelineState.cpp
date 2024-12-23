#include "GraphicsDevice.h"
#include "InputElementLayout.h"
#include "GraphicsPipelineState.h"
#include "RootSignature.h"
#include "Shader.h"

namespace sr
{
	namespace rendering
	{
		GraphicsPipelineState::GraphicsPipelineState(const RootSignature& rootSignature, InputElementLayout& inputElementLayout
			, const Shader& vertexShader, const Shader& pixelShader) noexcept
			: mD3d12GraphicsPipelineStateDescription({})
			, mD3d12PipelineState(nullptr)
		{
			mD3d12GraphicsPipelineStateDescription.pRootSignature = rootSignature.GetD3d12RootSignature();
			mD3d12GraphicsPipelineStateDescription.VS = vertexShader.GetShaderByteCode();
			mD3d12GraphicsPipelineStateDescription.PS = pixelShader.GetShaderByteCode();
			mD3d12GraphicsPipelineStateDescription.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			mD3d12GraphicsPipelineStateDescription.SampleMask = UINT_MAX;
			mD3d12GraphicsPipelineStateDescription.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			mD3d12GraphicsPipelineStateDescription.DepthStencilState.DepthEnable = FALSE;
			mD3d12GraphicsPipelineStateDescription.DepthStencilState.StencilEnable = FALSE;

			const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElementDescriptions = inputElementLayout.GetInputElementDescOwner();
			const UINT numInputElements = gsl::narrow_cast<UINT>(inputElementDescriptions.size()); // size_t == unsigned long long/ UINT == unsigned int
			mD3d12GraphicsPipelineStateDescription.InputLayout = { &gsl::at(inputElementDescriptions, 0),numInputElements }; // TODO: make Mesh class return Vertex-struct-related data
			
			mD3d12GraphicsPipelineStateDescription.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			
			// Only using 1 render target for simplicity
			mD3d12GraphicsPipelineStateDescription.NumRenderTargets = 2; // Changed to SwapChain.bufferCount
			mD3d12GraphicsPipelineStateDescription.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			mD3d12GraphicsPipelineStateDescription.SampleDesc.Count = 1;
		}

	}
}