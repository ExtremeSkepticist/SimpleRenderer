#include "Camera.h"
#include "ConstBuffer.h"
#include "DescriptorRange.h"
#include "FrameBuilder.h"
#include "GraphicsDevice.h"
#include "GraphicsPipelineState.h"
#include "Model.h"
#include "RenderTargetDescriptorHeap.h"
#include "RenderingPipeline.h"
#include "RootSignature.h"
#include "RootParameter.h"
#include "Scene.h"
#include "Shader.h"
#include "ShaderResourceDescriptorHeap.h"
#include "SwapChain.h"
#include "InputElementLayout.h"

namespace sr
{
	namespace rendering
	{
		FrameBuilder::FrameBuilder(RenderingPipeline& pipeline, Scene& scene)
			: mRootSignature(nullptr)
			, mPipelineState(nullptr)
			, mRenderTargetDescriptorHeap(make_unique<RenderTargetDescriptorHeap>())
			, mShaderResourceDescriptorHeap(make_unique<ShaderResourceDescriptorHeap>())
			, mModel(nullptr)
			, mDsvHeap(nullptr)
			, mConstBuffer(nullptr)
		{
			const GraphicsDevice& device = pipeline.GetGraphicsDevice();
			device.CreateDescriptorHeap(*mRenderTargetDescriptorHeap);
			device.CreateDescriptorHeap(*mShaderResourceDescriptorHeap);
			createDefaultResources(pipeline, scene);
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

		RenderTargetDescriptorHeap& FrameBuilder::GetRenderTargetDescriptorHeap()
		{
			return *mRenderTargetDescriptorHeap;
		}
		
		ShaderResourceDescriptorHeap& FrameBuilder::GetShaderResourceDescriptorHeap()
		{
			return *mShaderResourceDescriptorHeap;
		}

		GraphicsPipelineState& FrameBuilder::GetPipelineState()
		{
			return *mPipelineState;
		}

		RootSignature& FrameBuilder::GetRootSignature()
		{
			return *mRootSignature;
		}

		void FrameBuilder::Update()
		{

		}

		void FrameBuilder::createDefaultResources(RenderingPipeline& pipeline, Scene& scene)
		{
			// Create render target view
			sr::rendering::GraphicsDevice& device = pipeline.GetGraphicsDevice();
			sr::rendering::SwapChain& swapChain = pipeline.GetSwapChain();
			for (gsl::index i = 0; i < swapChain.GetBufferCount(); ++i)
			{
				sr::rendering::DescriptorHandle handle = mRenderTargetDescriptorHeap->GetHeapStartHandle();
				handle.cpuHandle.ptr += i * device.GetRenderTargetHeapHandleIncrementSize();

				sr::rendering::GpuResource resource;
				swapChain.GetDxgiSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource.GetD3d12ResourceOwner()));
				{
					// TODO: replace this with better solution
					std::wstring name(L"Render Target buffer %ld", i);
					resource.GetD3d12Resource()->SetName(name.c_str());
				}

				device.CreateRenderTargetView(resource, handle);
			}

			// TODO: set the following values with the .gltf provided values
			// Create const buffer
			const DirectX::XMFLOAT3 modelWorldPosition = scene.GetModel().GetWorldCoords();
			XMMATRIX worldMatrix = XMMatrixMultiply(XMMatrixScaling(0.8f, 0.8f, 0.8f), XMMatrixRotationX(-0.8));
			worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(1));

			worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(modelWorldPosition.x, modelWorldPosition.y, modelWorldPosition.z));
			worldMatrix = XMMatrixTranspose(worldMatrix);

			sr::rendering::Camera& camera = scene.GetCamera();
			const XMMATRIX viewMatrix = XMMatrixTranspose(camera.GetViewLookAtMatrixLh(camera.GetWorldCoords(), modelWorldPosition, sr::rendering::worldUpDirection));
			const XMMATRIX projectionMatrix = XMMatrixTranspose(camera.GetProjectionMatrixLh());
			XMMATRIX lightDirections = {};
			{
				const XMFLOAT3 lightDir = { 0, 6 ,2 };
				lightDirections.r[0] = XMLoadFloat3(&lightDir);
			}

			//constexpr XMFLOAT3 pointLightSourceWorldPosition(modelWorldPosition.x, modelWorldPosition.y + 3, modelWorldPosition.z);
			// TODO: Store the max light intensity somewhere
			// const sr::rendering::PointLight lightSource = { XMFLOAT3(1, 1, 1), 100, pointLightSourceWorldPosition };

			const sr::rendering::CpuConstBufferData mappableConstBuffer{ worldMatrix, viewMatrix, projectionMatrix, lightDirections };
			mConstBuffer = make_unique<sr::rendering::ConstBuffer>(L"Coord Transform Const buffer", mappableConstBuffer, pipeline.GetGraphicsDevice());

			sr::rendering::DescriptorHandle descriptorHandle = mShaderResourceDescriptorHeap->GetHeapStartHandle();
			device.CreateConstBufferView(*mConstBuffer, descriptorHandle);
		}

		void FrameBuilder::createDefaultRootSignature(const GraphicsDevice& device)
		{
			// Initialize RootSignature
			constexpr size_t numDescriptorRanges = 2;
			constexpr size_t numConstBufferDescriptors = 1;
			constexpr size_t numShaderResourceDescriptors = 3; // albedo + normal + metallicRoughness
			sr::rendering::DescriptorRange descriptorRanges[numDescriptorRanges] = {
				{ sr::rendering::DescriptorRange::RangeType::ConstBuffer, numConstBufferDescriptors }
				, { sr::rendering::DescriptorRange::RangeType::ShaderResourceBuffer, numShaderResourceDescriptors }
			};

			constexpr size_t numRootParameters = 1;
			sr::rendering::RootParameter rootParameters[numRootParameters] = {
				{ descriptorRanges }
			};

			mRootSignature = make_unique<RootSignature>(rootParameters);
			device.CreateRootSignature(*mRootSignature);
		}

		void FrameBuilder::createDefaultPipelineState(const GraphicsDevice& device)
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
			inputElementLayout.AddLayout("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT);
			inputElementLayout.AddLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			inputElementLayout.AddLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
			inputElementLayout.AddLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

			mPipelineState = make_unique<GraphicsPipelineState>(*mRootSignature, inputElementLayout, vertexShader, pixelShader);
			device.CreateGraphicsPipelineState(*mPipelineState);
		}
	}
}