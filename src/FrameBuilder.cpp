#include "FrameBuilder.h"

#include "Camera.h"
#include "Entity.h"
#include "ConstBuffer.h"
#include "DescriptorRange.h"
#include "GraphicsDevice.h"
#include "GraphicsPipelineState.h"
#include "Mesh.h"
#include "RenderTargetDescriptorHeap.h"
#include "ShaderResourceDescriptorHeap.h" // Definition order for unique_ptr
#include "RenderingPipeline.h"
#include "RootSignature.h"
#include "RootParameter.h"
#include "Scene.h"
#include "Shader.h"
#include "SwapChain.h"
#include "InputElementLayout.h"
#include "TextureGroup.h";

namespace sr
{
	namespace rendering
	{
		FrameBuilder::FrameBuilder(RenderingPipeline& pipeline, Scene& scene)
			: mRootSignature(nullptr)
			, mPipelineState(nullptr)
			, mRenderTargetDescriptorHeap(make_unique<RenderTargetDescriptorHeap>())
			, mShaderResourceDescriptorHeap(make_unique<ShaderResourceDescriptorHeap>())
			, mDsvHeap(nullptr)
			, mConstBuffer(nullptr)
			, mVertexResourceMap()
			, mIndexResourceMap()
		{
			// TODO: disable some stuff that's not required for solid color rendering?
			const GraphicsDevice& device = pipeline.GetGraphicsDevice();
			device.CreateDescriptorHeap(*mRenderTargetDescriptorHeap);
			device.CreateDescriptorHeap(*mShaderResourceDescriptorHeap);
			createDefaultResources(pipeline);
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
			
			const uint16_t frameWidth = pipeline.GetSwapChain().GetBufferWidth();
			const uint16_t frameHeight = pipeline.GetSwapChain().GetBufferHeight();
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

		FrameBuilder::~FrameBuilder() = default;

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

		D3D12_VERTEX_BUFFER_VIEW& FrameBuilder::GetVertexBufferView(Mesh& mesh)
		{
			return mVertexResourceMap[&mesh].second;

		}
		
		D3D12_INDEX_BUFFER_VIEW& FrameBuilder::GetIndexBufferView(Mesh& mesh)
		{
			return mIndexResourceMap[&mesh].second;
		}

		void FrameBuilder::Build(Mesh& mesh, RenderableNode& node, Camera& camera, XMFLOAT3 targetPosition, GraphicsDevice& device) noexcept
		{
			// TODO: Create DEFAULT heap and copy these to it
			// Initialize vertex buffer

			const size_t vertexDataSize = sizeof(Mesh::Vertex) * mesh.GetVerticeCount();
			if (mVertexResourceMap.find(&mesh) == mVertexResourceMap.end())
			{
				mVertexResourceMap[&mesh] = std::make_pair<GpuResource, D3D12_VERTEX_BUFFER_VIEW>(
					GpuResource(L"Vertex buffer resource", vertexDataSize, ResourceHeapType::Upload, ResourceState::GenericRead)
					, D3D12_VERTEX_BUFFER_VIEW()
				);

				auto& vertexPair = mVertexResourceMap[&mesh];
				GpuResource& resource = vertexPair.first;
				D3D12_VERTEX_BUFFER_VIEW& view = vertexPair.second;
				device.CreateGpuResource(resource);
				{
					// TODO: abstract the mapping process if possible
					const auto& vertices = mesh.GetVertice();
					{
						uint8_t* mappedMemoryForVertexBuffer = nullptr;
						CD3DX12_RANGE readRange(0, 0);
						ComPtr<ID3D12Resource>& vertexBufferOwner = resource.GetD3d12ResourceOwner();

						vertexBufferOwner->Map(0, &readRange, reinterpret_cast<void**>(&mappedMemoryForVertexBuffer));
						memcpy(mappedMemoryForVertexBuffer, &vertices[0], vertexDataSize);
						vertexBufferOwner->Unmap(0, nullptr);

						view.BufferLocation = vertexBufferOwner->GetGPUVirtualAddress();
						view.StrideInBytes = sizeof(Mesh::Vertex);
						view.SizeInBytes = vertexDataSize;
					}

				}
			}
			if (mIndexResourceMap.find(&mesh) == mIndexResourceMap.end())
			{
				mIndexResourceMap[&mesh] = std::make_pair<GpuResource, D3D12_INDEX_BUFFER_VIEW>(
					GpuResource(L"Index buffer resource", vertexDataSize, ResourceHeapType::Upload, ResourceState::GenericRead)
					, D3D12_INDEX_BUFFER_VIEW()
				);

				auto& indexPair = mIndexResourceMap[&mesh];
				GpuResource& resource = indexPair.first;
				D3D12_INDEX_BUFFER_VIEW& view = indexPair.second;

				// Initialize index buffer
				const size_t indexDataSize = mesh.GetIndicesCount() * indexSize;
				device.CreateGpuResource(resource);
				{
					const auto& indices = mesh.GetIndice();
					{
						uint8_t* mappedMemoryForIndexBuffer = nullptr;
						CD3DX12_RANGE readRange(0, 0);
						ComPtr<ID3D12Resource>& indexBufferOwner = resource.GetD3d12ResourceOwner();

						indexBufferOwner->Map(0, &readRange, reinterpret_cast<void**>(&mappedMemoryForIndexBuffer));
						memcpy(mappedMemoryForIndexBuffer, &indices[0], indexDataSize);
						indexBufferOwner->Unmap(0, nullptr);

						view.BufferLocation = indexBufferOwner->GetGPUVirtualAddress();
						view.SizeInBytes = indexDataSize;
						view.Format = indexFormat;
					}
				}
			}

			// TODO: build const buffer
			// TODO: set the following values with the .gltf provided values
			const XMMATRIX worldMatrix = XMMatrixTranspose(node.GetWorldMatrix());
			const XMMATRIX viewMatrix = XMMatrixTranspose(camera.GetViewLookAtMatrixLh(camera.GetPosition(), targetPosition, sr::rendering::worldUpDirection));
			const XMMATRIX projectionMatrix = XMMatrixTranspose(camera.GetProjectionMatrixLh());

			const CpuConstBufferData data = { worldMatrix, viewMatrix, projectionMatrix, XMMatrixIdentity()};
			mConstBuffer->Update(data);

			//
			//XMMATRIX lightDirections = {};
			//{
			//	const XMFLOAT3 lightDir = { 0, 6 ,2 };
			//	lightDirections.r[0] = XMLoadFloat3(&lightDir);
			//}

			//constexpr XMFLOAT3 pointLightSourceWorldPosition(modelWorldPosition.x, modelWorldPosition.y + 3, modelWorldPosition.z);
			// TODO: Store the max light intensity somewhere
			// const sr::rendering::PointLight lightSource = { XMFLOAT3(1, 1, 1), 100, pointLightSourceWorldPosition };
		}

		void FrameBuilder::createDefaultResources(RenderingPipeline& pipeline) noexcept
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

			// Create const buffer
			mConstBuffer = make_unique<sr::rendering::ConstBuffer>(L"Coord Transform Const buffer", pipeline.GetGraphicsDevice());
			sr::rendering::DescriptorHandle descriptorHandle = mShaderResourceDescriptorHeap->GetHeapStartHandle();
			device.CreateConstBufferView(*mConstBuffer, descriptorHandle);
		}

		void FrameBuilder::createDefaultRootSignature(const GraphicsDevice& device)
		{
			// Initialize RootSignature
			constexpr size_t numDescriptorRanges = 1;
			constexpr size_t numConstBufferDescriptors = 1;
			// constexpr size_t numShaderResourceDescriptors = 3; // albedo + normal + metallicRoughness
			sr::rendering::DescriptorRange descriptorRanges[numDescriptorRanges] = {
				{ sr::rendering::DescriptorRange::RangeType::ConstBuffer, numConstBufferDescriptors }
				// , { sr::rendering::DescriptorRange::RangeType::ShaderResourceBuffer, numShaderResourceDescriptors }
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

		void FrameBuilder::bindSrvResources(gsl::not_null<TextureGroup*> textureGroup, const GraphicsDevice& device) noexcept
		{
			//Expects(mConstBuffer != nullptr);

			//sr::rendering::DescriptorHandle descriptorHandle = mShaderResourceDescriptorHeap->GetHeapStartHandle();
			//device.CreateConstBufferView(*mConstBuffer, descriptorHandle);
			//descriptorHandle.cpuHandle.ptr += device.GetShaderResourceDescriptorHeapHandleIncrementSize();

			//// TODO: add emissive & AO map
			//constexpr size_t textureNum = static_cast<size_t>(Texture::TextureType::Num) - 2;
			//for (gsl::index i = 0; i < textureNum; ++i)
			//{
			//	Texture& texture = textureGroup->GetTexture(mMesh, static_cast<Texture::TextureType>(i));
			//	
			//	device.CreateShaderResourceView(texture, descriptorHandle);

			//	descriptorHandle.cpuHandle.ptr += device.GetShaderResourceDescriptorHeapHandleIncrementSize();
			//}
		}

	}
}