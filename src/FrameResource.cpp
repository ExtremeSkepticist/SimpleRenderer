#include "DescriptorRange.h"
#include "FrameResource.h"
#include "GraphicsDevice.h"
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
		FrameResource::FrameResource(const GraphicsDevice& device)
			: mRenderTargetDescriptorHeap(make_unique<RenderTargetDescriptorHeap>())
			, mRootSignature(nullptr)
			, mPipelineState(nullptr)
			, mShaderResourceDescriptorHeap(make_unique< ShaderResourceDescriptorHeap>())
			, mModel(nullptr)
		{
			device.CreateDescriptorHeap(*mRenderTargetDescriptorHeap);
			createDefaultRootSignature(device);
			createDefaultRootSignature(device);
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