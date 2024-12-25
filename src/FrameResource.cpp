#include "DescriptorRange.h"
#include "FrameResource.h"
#include "GraphicsDevice.h"
#include "GraphicsPipelineState.h"
#include "RenderTargetDescriptorHeap.h"
#include "RootSignature.h"
#include "RootParameter.h"
#include "Shader.h"
#include "ShaderResourceDescriptorHeap.h"
#include "InputElementLayout.h"

namespace sr
{
	namespace rendering
	{
		// TODO: Create depth-stencil buffer and heap to bind
		FrameResource::FrameResource(const GraphicsDevice& device)
			: mRenderTargetDescriptorHeap(make_unique<RenderTargetDescriptorHeap>())
			, mRootSignature(nullptr)
			, mPipelineState(nullptr)
			, mShaderResourceDescriptorHeap(make_unique< ShaderResourceDescriptorHeap>())
			, mModel(nullptr)
			, mDsvHeap(nullptr)
		{
			device.CreateDescriptorHeap(*mRenderTargetDescriptorHeap);
			device.CreateDescriptorHeap(*mShaderResourceDescriptorHeap);
			createDefaultRootSignature(device);
			createDefaultPipelineState(device);

			// TODO: abstract this
			D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
			dsvDesc.NumDescriptors = 1;
			dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			device.GetD3d12Device()->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&mDsvHeap));
			Ensures(mDsvHeap != nullptr);

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvViewDesc = {};
			dsvViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsvViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvViewDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
			depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0;
			
			constexpr uint16_t frameWidth = 800;
			constexpr uint16_t frameHeight = 600;
			auto props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			auto desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, frameWidth, frameHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
			device.GetD3d12Device()->CreateCommittedResource(
				&props, D3D12_HEAP_FLAG_NONE, &desc
				, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue
				, IID_PPV_ARGS(&mDsvBuffer)
			);
			assert(mDsvBuffer != nullptr);
		
			device.GetD3d12Device()->CreateDepthStencilView(mDsvBuffer.Get(), &dsvViewDesc, mDsvHeap->GetCPUDescriptorHandleForHeapStart());
		}

		RenderTargetDescriptorHeap& FrameResource::GetRenderTargetDescriptorHeap()
		{
			return *mRenderTargetDescriptorHeap;
		}
		
		ShaderResourceDescriptorHeap& FrameResource::GetShaderResourceDescriptorHeap()
		{
			return *mShaderResourceDescriptorHeap;
		}

		GraphicsPipelineState& FrameResource::GetPipelineState()
		{
			return *mPipelineState;
		}

		RootSignature& FrameResource::GetRootSignature()
		{
			return *mRootSignature;
		}

		void FrameResource::createDefaultRootSignature(const GraphicsDevice& device)
		{
			// Initialize RootSignature
			constexpr size_t numDescriptorRanges = 2;
			constexpr size_t numConstBufferDescriptors = 1;
			constexpr size_t numShaderResourceBufferDescriptors = 1;
			sr::rendering::DescriptorRange descriptorRanges[numDescriptorRanges] = {
				{ sr::rendering::DescriptorRange::RangeType::ConstBuffer, numConstBufferDescriptors }
				, { sr::rendering::DescriptorRange::RangeType::ShaderResourceBuffer, numShaderResourceBufferDescriptors }
			};

			constexpr size_t numRootParameters = 1;
			sr::rendering::RootParameter rootParameters[numRootParameters] = {
				{ descriptorRanges }
			};

			mRootSignature = make_unique<RootSignature>(rootParameters);
			device.CreateRootSignature(*mRootSignature);
		}

		void FrameResource::createDefaultPipelineState(const GraphicsDevice& device)
		{
			// TODO: Use batch script instead to compile shaders
			// TODO: Implement changing shaders upon using different effects

			constexpr gsl::cwzstring vertexShaderPath = L"shaders\\BasicVertexShader.cso";
			const sr::rendering::Shader vertexShader(vertexShaderPath);
			constexpr gsl::cwzstring pixelShaderPath = L"shaders\\BasicPixelShader.cso";
			const sr::rendering::Shader pixelShader(pixelShaderPath);

			// Initialize input element layout
			constexpr uint32_t inputSlot = 0;
			sr::rendering::InputElementLayout inputElementLayout(inputSlot);
			inputElementLayout.AddLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			inputElementLayout.AddLayout("TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT);
			inputElementLayout.AddLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			inputElementLayout.AddLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			inputElementLayout.AddLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

			mPipelineState = make_unique<GraphicsPipelineState>(*mRootSignature, inputElementLayout, vertexShader, pixelShader);
			device.CreateGraphicsPipelineState(*mPipelineState);
		}
	}
}