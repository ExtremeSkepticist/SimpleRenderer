#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include "DescriptorRange.h"
#include "GraphicsCommandQueue.h"
#include "GraphicsInfrastructure.h"
#include "GraphicsDevice.h"
#include "GpuResource.h"
#include "InputElementLayout.h"
#include "PlatformWindow.h"
#include "RenderTargetDescriptorHeap.h"
#include "RootParameter.h"
#include "RootSignature.h"
#include "Shader.h"
#include "SwapChain.h"

#include <DirectXTex.h>

#include "Camera.h"
#include "Common.h"
#include "Model.h"

using namespace Microsoft::WRL;

typedef struct
{
	XMMATRIX mWorldMatrix;
	XMMATRIX mViewMatrix;
	XMMATRIX mProjectionMatrix;
	XMMATRIX mUnused;
} ConstBuffer;

HWND g_hWnd = nullptr; //TODO: should be removed and replaced with getter from PlatformWindow

// D3D12 resources
const size_t g_FRAME_WIDTH = 800;
const size_t g_FRAME_HEIGHT = 600;
const size_t g_FRAME_COUNT = 2;
size_t g_CurrentFrameIndex = 0;
CD3DX12_VIEWPORT g_Viewport(0.0f, 0.0f, static_cast<float>(g_FRAME_WIDTH), static_cast<float>(g_FRAME_HEIGHT));
CD3DX12_RECT g_ScissorRect(0, 0, static_cast<LONG>(g_FRAME_WIDTH), static_cast<LONG>(g_FRAME_HEIGHT));
ComPtr<IDXGISwapChain3> g_pSwapChain = nullptr;
ComPtr<ID3D12Resource> g_pRenderTargets[g_FRAME_COUNT];
ComPtr<ID3D12Device> g_pDevice = nullptr;
ComPtr<ID3D12DescriptorHeap> g_pRenderTargetViewHeap = nullptr;
ComPtr<ID3D12DescriptorHeap> g_pDescriptorHeap = nullptr;
ComPtr<ID3D12Resource> g_pConstBuffer = nullptr;
ComPtr<ID3D12CommandAllocator> g_pCmdAllocator = nullptr;
ComPtr<ID3D12GraphicsCommandList> g_pCmdList = nullptr;
ComPtr<ID3D12CommandQueue> g_pCmdQueue = nullptr;
ComPtr<ID3D12RootSignature> g_pRootSignature = nullptr;
ComPtr<ID3D12PipelineState> g_pPipelineStateObj = nullptr;
ComPtr<ID3D12Fence> g_pFence = nullptr;
UINT64 g_FenceValue = 1;
HANDLE g_FenceEvent = nullptr;

ComPtr<ID3D12Resource> g_pAlbedoTexture = nullptr;
ComPtr<ID3D12Resource> g_pAlbedoUploadHeap = nullptr;

// Model related variables
Model* g_pModel = nullptr;
const XMFLOAT3 g_MODEL_WORLD_POSITION(0, 4, 2);

// Camera related variables
const XMFLOAT3 g_CAMERA_WORLD_POSITION(0, 6, 0);
const XMFLOAT3 g_WORLD_UP_DIRECITON(0, 1, 0);
constexpr float g_FOV_ANGLE = 45.0f;
constexpr float g_NEAR_Z_COORD = 0.01f;
constexpr float g_FAR_Z_COORD = 100.0f;
Camera g_Camera(g_FOV_ANGLE, g_FRAME_WIDTH / g_FRAME_HEIGHT, g_NEAR_Z_COORD, g_FAR_Z_COORD, g_CAMERA_WORLD_POSITION);

ConstBuffer g_ConstBufferData;
UINT8* g_pConstBufferMapped = nullptr;

void LoadRenderResources();
void WaitForPreviousFrame();

void Init(HINSTANCE hInstance, int nCmdShow);
void Update();
void Render();


// TODO: deprecate d3dx12 and implement its own initialization
int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	Init(hInstance, nCmdShow);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("resources\\DamagedHelmet.gltf", aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
	assert(scene != nullptr);

	// TODO: Implement recursive initialization when needed
	Model model(*scene, g_MODEL_WORLD_POSITION);
	g_pModel = &model;

	LoadRenderResources();
	
	while (true) {
		Render();
	}

	return 0;
}

// TODO: Encapsulate all directx 12, WinApi logic inside a wrapper class
void Init(HINSTANCE hInstance, int nCmdShow)
{
	Expects(hInstance != nullptr);

#pragma region Init Platform
	LoadLibrary(L"C:\\Program Files\\Microsoft PIX\\2405.15.002-OneBranch_release\\WinPixGpuCapturer.dll");

	const HRESULT isComInitialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	assert(SUCCEEDED(isComInitialized));

	constexpr uint16_t frameWidth = 800;
	constexpr uint16_t frameHeight = 600;
	sr::platform::PlatformWindow platformWindow(frameWidth, frameHeight, L"SimpleRenderer", hInstance, nCmdShow);
	{
		g_hWnd = platformWindow.GetWindowHandle(); // TODO: should be removed and replaced
	}
#pragma endregion

// TODO: Rendering Pipeline Class that contains below
#pragma region Init Rendering Pipeline
	constexpr UINT infrastructureFlag = 0;
	sr::rendering::GraphicsInfrastructure graphicsInfrastructure(infrastructureFlag);
	sr::rendering::GraphicsDevice graphicsDevice(graphicsInfrastructure);
	{
		// TODO: to be removed
		g_pDevice = graphicsDevice.GetD3d12Device();
		assert(g_pDevice != nullptr);
	}	
	
	sr::rendering::GraphicsCommandQueue graphicsCommandQueue(sr::rendering::GraphicsCommandQueueType::Graphics);
	graphicsDevice.CreateCommandQueue(graphicsCommandQueue, graphicsInfrastructure);
	{
		// TODO: to be removed
		g_pCmdQueue = graphicsCommandQueue.GetD3d12CommandQueue();
		assert(g_pCmdQueue != nullptr);
	}

	// Create SwapChain
	constexpr uint32_t swapChainBufferCount = 2;
	sr::rendering::SwapChain swapChain(swapChainBufferCount, platformWindow, graphicsCommandQueue, graphicsInfrastructure);
	{
		// TODO: to be removed
		g_pSwapChain = swapChain.GetDxgiSwapChain();
		assert(g_pSwapChain != nullptr);
	}

	// Create RenderTargetHeap
	sr::rendering::RenderTargetDescriptorHeap renderTargetDescriptorHeap(swapChainBufferCount, swapChain, graphicsDevice);
	{
		// TODO: to be removed
		g_pRenderTargetViewHeap = renderTargetDescriptorHeap.GetDescriptorHeapOwnership();
		g_pRenderTargets[0] = gsl::at(renderTargetDescriptorHeap.GetGpuResources(), 0).GetD3d12ResourceOwner();
		g_pRenderTargets[1] = gsl::at(renderTargetDescriptorHeap.GetGpuResources(), 1).GetD3d12ResourceOwner();

		assert(g_pRenderTargetViewHeap != nullptr);
		assert(g_pRenderTargets[0] != nullptr);
		assert(g_pRenderTargets[1] != nullptr);
	}

	// Initialize RootSignature
	constexpr size_t numDescriptorRanges = 2;
	sr::rendering::DescriptorRange descriptorRanges[numDescriptorRanges] = {
		{ sr::rendering::DescriptorRange::RangeType::ConstBuffer, 1 }
		, { sr::rendering::DescriptorRange::RangeType::ShaderResourceBuffer, 1 }
	};

	constexpr size_t numRootParameters = 1;
	sr::rendering::RootParameter rootParameters[numRootParameters] = {
		{ descriptorRanges }
	};

	sr::rendering::RootSignature rootSignature(rootParameters);
	graphicsDevice.CreateRootSignature(rootSignature);
	{
		// TODO: to be removed
		g_pRootSignature = rootSignature.GetD3d12RootSignatureOwner();
		assert(g_pRootSignature != nullptr);
	}
#pragma endregion
}

void Render()
{
	HRESULT result = g_pCmdAllocator->Reset();
	assert(SUCCEEDED(result));

	result = g_pCmdList->Reset(g_pCmdAllocator.Get(), g_pPipelineStateObj.Get());
	assert(SUCCEEDED(result));

	g_pCmdList->SetGraphicsRootSignature(g_pRootSignature.Get());

	ID3D12DescriptorHeap* pArrayDescriptorHeaps[] = { g_pDescriptorHeap.Get() };
	g_pCmdList->SetDescriptorHeaps(_countof(pArrayDescriptorHeaps), pArrayDescriptorHeaps);

	g_pCmdList->SetGraphicsRootDescriptorTable(0, g_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	g_pCmdList->RSSetViewports(1, &g_Viewport);
	g_pCmdList->RSSetScissorRects(1, &g_ScissorRect);
	
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(g_pRenderTargets[g_CurrentFrameIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	g_pCmdList->ResourceBarrier(1, &resourceBarrier);
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), g_CurrentFrameIndex, g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		g_pCmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		const FLOAT pClearingColor[] = { 0, 0, 0, 0 };
		g_pCmdList->ClearRenderTargetView(rtvHandle, pClearingColor, 0, nullptr);
		g_pCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		const size_t MESH_COUNT = g_pModel->GetMeshes().size();
		auto& meshes = g_pModel->GetMeshes();
		for (size_t i = 0; i < MESH_COUNT; ++i)
		{
			g_pCmdList->IASetVertexBuffers(0, 1, &meshes[i]->GetVertexBufferView());
			g_pCmdList->IASetIndexBuffer(&meshes[i]->GetIndexBufferView());
			g_pCmdList->DrawIndexedInstanced(meshes[i]->GetIndexNum(), 1, 0, 0, 0);
		}
	}
	resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(g_pRenderTargets[g_CurrentFrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	g_pCmdList->ResourceBarrier(1, &resourceBarrier);
	
	g_pCmdList->Close();

	ID3D12CommandList* cmdListArray[]  = { g_pCmdList.Get()};
	g_pCmdQueue->ExecuteCommandLists(_countof(cmdListArray), cmdListArray);

	g_pSwapChain->Present(1, 0);

	WaitForPreviousFrame();
}

void LoadRenderResources()
{
#pragma region Initialize shader
	// TODO: Use batch script instead to compile shaders
	// TODO: Implement changing shaders upon using different effects
	constexpr gsl::cwzstring vertexShaderPath = L"shaders\\BasicVertexShader.cso";
	const sr::rendering::Shader vertexShader(vertexShaderPath);
	constexpr gsl::cwzstring pixelShaderPath = L"shaders\\BasicPixelShader.cso";
	const sr::rendering::Shader pixelShader(pixelShaderPath);
#pragma endregion

#pragma region Initialize PSO
	// Initialize PSO
	constexpr uint32_t inputSlot = 0;
	sr::rendering::InputElementLayout inputElementLayout(inputSlot);
	inputElementLayout.AddLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	inputElementLayout.AddLayout("TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT);
	inputElementLayout.AddLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	inputElementLayout.AddLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	inputElementLayout.AddLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElementDescsOwner = inputElementLayout.GetInputElementDescOwner();
	const uint32_t numInputElements = inputElementDescsOwner.size();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0, };
	psoDesc.InputLayout = { &gsl::at(inputElementDescsOwner, 0), numInputElements }; // TODO: make Mesh class return Vertex-struct-related data
	psoDesc.pRootSignature = g_pRootSignature.Get();
	psoDesc.VS = vertexShader.GetShaderByteCode();
	psoDesc.PS = pixelShader.GetShaderByteCode();
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	g_pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&g_pPipelineStateObj));
	assert(g_pPipelineStateObj != nullptr);

#pragma endregion

#pragma region Create command allocator & command list
	g_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_pCmdAllocator));
	assert(g_pCmdAllocator != nullptr);

	g_pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT
		, g_pCmdAllocator.Get(), g_pPipelineStateObj.Get(), IID_PPV_ARGS(&g_pCmdList));
	assert(g_pCmdList != nullptr);
	g_pCmdList->Close();
#pragma endregion

#pragma region Create descriptor heap
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.NumDescriptors = 2;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	g_pDevice->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&g_pDescriptorHeap));
	assert(g_pDescriptorHeap != nullptr);
#pragma endregion

#pragma region Create Const Buffer
	const size_t CONST_BUFFER_SIZE = sizeof(ConstBuffer);
	const CD3DX12_HEAP_PROPERTIES CONST_BUFFER_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	const CD3DX12_RESOURCE_DESC CONST_BUFFER_RESOURCE_DESC = CD3DX12_RESOURCE_DESC::Buffer(CONST_BUFFER_SIZE);
	g_pDevice->CreateCommittedResource(
		&CONST_BUFFER_HEAP_PROPERTIES
		, D3D12_HEAP_FLAG_NONE
		, &CONST_BUFFER_RESOURCE_DESC
		, D3D12_RESOURCE_STATE_GENERIC_READ
		, nullptr
		, IID_PPV_ARGS(&g_pConstBuffer)
	);
	g_pConstBuffer->SetName(L"Const buffer resource");

	D3D12_CONSTANT_BUFFER_VIEW_DESC constBufferDesc = { 0, };
	constBufferDesc.BufferLocation = g_pConstBuffer->GetGPUVirtualAddress();
	constBufferDesc.SizeInBytes = CONST_BUFFER_SIZE;
	g_pDevice->CreateConstantBufferView(&constBufferDesc, g_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	XMMATRIX worldMatrix = XMMatrixMultiply(XMMatrixScaling(0.8f, 0.8f, 0.8f), XMMatrixRotationX(-0.8));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(-0.1));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(g_MODEL_WORLD_POSITION.x, g_MODEL_WORLD_POSITION.y, g_MODEL_WORLD_POSITION.z));

	g_ConstBufferData.mWorldMatrix = XMMatrixTranspose(worldMatrix);
	g_ConstBufferData.mViewMatrix = XMMatrixTranspose(g_Camera.GetViewLookAtMatrixLh(g_CAMERA_WORLD_POSITION, g_MODEL_WORLD_POSITION, g_WORLD_UP_DIRECITON));
	g_ConstBufferData.mProjectionMatrix = XMMatrixTranspose(g_Camera.GetProjectionMatrixLh());

	// Map and initialize the constant buffer. We don't unmap this until the
	// app closes. Keeping things mapped for the lifetime of the resource is okay.
	CD3DX12_RANGE constBufferReadRange(0, 0);
	g_pConstBuffer->Map(0, &constBufferReadRange, reinterpret_cast<void**>(&g_pConstBufferMapped));
	memcpy(g_pConstBufferMapped, &g_ConstBufferData, sizeof(g_ConstBufferData));

#pragma endregion

#pragma region Create Albedo Texture
	g_pCmdAllocator->Reset();
	g_pCmdList->Reset(g_pCmdAllocator.Get(), nullptr);

	TexMetadata imageMetadata = {};
	ScratchImage scratchImage = {};
	HRESULT hasLoaded = LoadFromDDSFile(L"resources\\albedo.dds", DDS_FLAGS_NONE, &imageMetadata, scratchImage);
	{
		const Image* image = scratchImage.GetImage(0, 0, 0);

		// Create default heap for albedo texture
		CD3DX12_RESOURCE_DESC defaultHeapDesc = CD3DX12_RESOURCE_DESC::Tex2D(image->format, image->width, image->height);
		CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
		g_pDevice->CreateCommittedResource(
			&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &defaultHeapDesc
			, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&g_pAlbedoTexture));
		g_pAlbedoTexture->SetName(L"Albedo texture - default");

		// Create upload heap for albedo texture
		CD3DX12_HEAP_PROPERTIES uploadProps(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(GetRequiredIntermediateSize(g_pAlbedoTexture.Get(), 0, 1));
		g_pDevice->CreateCommittedResource(
			&uploadProps, D3D12_HEAP_FLAG_NONE, &uploadResourceDesc
			, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&g_pAlbedoUploadHeap));
		g_pAlbedoUploadHeap->SetName(L"Albedo texture - upload");

		CD3DX12_RANGE uploadReadRange(0, 0);
		uint8_t* mappedImageData = nullptr;
		g_pAlbedoUploadHeap->Map(0, &uploadReadRange, reinterpret_cast<void**>(&mappedImageData));
		{
			memcpy(mappedImageData, image->pixels, image->slicePitch);
		}
		g_pAlbedoUploadHeap->Unmap(0, nullptr);

		// Copy from upload to default heap
		D3D12_SUBRESOURCE_DATA subresourceData = {};
		subresourceData.pData = image->pixels;
		subresourceData.RowPitch = image->width * sizeof(uint32_t); // R8G8B8A8 format per pixel
		subresourceData.SlicePitch = image->height * subresourceData.RowPitch;

		UpdateSubresources(g_pCmdList.Get(), g_pAlbedoTexture.Get(), g_pAlbedoUploadHeap.Get(), 0, 0, 1, &subresourceData);

		auto transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
			g_pAlbedoTexture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		g_pCmdList->ResourceBarrier(1, &transitionBarrier);


		// TODO: Add ShaderResourceDescriptorHeap
		// Create SRV
		D3D12_SHADER_RESOURCE_VIEW_DESC srvViewDesc = {};
		srvViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvViewDesc.Format = image->format;
		srvViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvViewDesc.Texture2D.MipLevels = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = g_pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		srvHandle.ptr += g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		g_pDevice->CreateShaderResourceView(g_pAlbedoTexture.Get(), &srvViewDesc, srvHandle);
	
		g_pCmdList->Close();

		ID3D12CommandList* cmdListArray[] = { g_pCmdList.Get() };
		g_pCmdQueue->ExecuteCommandLists(_countof(cmdListArray), cmdListArray);

		WaitForPreviousFrame();
	}

	scratchImage.Release();
#pragma endregion

#pragma region Create Vertex & Index Buffer
	auto& meshes = g_pModel->GetMeshes();
	for (auto& mesh : meshes)
	{
		// TODO: Use DEFAULT heap instead
		const size_t VERTEX_BUFFER_SIZE = sizeof(Mesh::Vertex) * mesh->GetVerticeNum();
		CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(VERTEX_BUFFER_SIZE);

		HRESULT isVertexBufferCreated = g_pDevice->CreateCommittedResource(
			&heapProp
			, D3D12_HEAP_FLAG_NONE
			, &vertexBufferDesc
			, D3D12_RESOURCE_STATE_GENERIC_READ
			, nullptr
			, IID_PPV_ARGS(&mesh->GetVertexBuffer()));

		assert(SUCCEEDED(isVertexBufferCreated));
		
		// TODO: Set every resource name
		mesh->GetVertexBuffer()->SetName(L"Vertex buffer resource");

		// TODO: Figure out how to set index size cleanly
		const size_t INDEX_SIZE = sizeof(UINT32);
		const size_t INDEX_BUFFER_SIZE = mesh->GetIndexNum() * INDEX_SIZE;
		CD3DX12_RESOURCE_DESC indexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(INDEX_BUFFER_SIZE);
		bool isIndexBufferCreated = g_pDevice->CreateCommittedResource(
			&heapProp
			, D3D12_HEAP_FLAG_NONE
			, &indexBufferDesc
			, D3D12_RESOURCE_STATE_GENERIC_READ
			, nullptr
			, IID_PPV_ARGS(&mesh->GetIndexBuffer())
		);
		assert(SUCCEEDED(isIndexBufferCreated));

		UINT8* ptrVertexDataMapped = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		mesh->GetVertexBuffer()->Map(0, &readRange, reinterpret_cast<void**>(&ptrVertexDataMapped));
		{
			memcpy(ptrVertexDataMapped, &mesh->GetVerticeData()[0], VERTEX_BUFFER_SIZE);
		}
		mesh->GetVertexBuffer()->Unmap(0, nullptr);

		UINT8* ptrIndexDataMapped = nullptr;
		mesh->GetIndexBuffer()->Map(0, &readRange, reinterpret_cast<void**>(&ptrIndexDataMapped));
		{
			memcpy(ptrIndexDataMapped, &mesh->GetIndexData()[0], INDEX_BUFFER_SIZE);
		}
		mesh->GetIndexBuffer()->Unmap(0, nullptr);

		D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = mesh->GetVertexBufferView();
		vertexBufferView.BufferLocation = mesh->GetVertexBuffer()->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Mesh::Vertex);
		vertexBufferView.SizeInBytes = VERTEX_BUFFER_SIZE;

		D3D12_INDEX_BUFFER_VIEW& indexBufferView = mesh->GetIndexBufferView();
		indexBufferView.BufferLocation = mesh->GetIndexBuffer()->GetGPUVirtualAddress();
		indexBufferView.SizeInBytes = INDEX_BUFFER_SIZE;
		indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	}
#pragma endregion
}

//TODO: lOOK AT D3D12HellOFrameBuffering
void WaitForPreviousFrame()
{
	const UINT CURRENT_FENCE_VALUE = g_FenceValue++;
	g_pCmdQueue->Signal(g_pFence.Get(), CURRENT_FENCE_VALUE);

	// Wait until fence-signalling command is run by command queue
	if (g_pFence->GetCompletedValue() < CURRENT_FENCE_VALUE)
	{
		// Wait until fence event is run, after fence reached the specified value
		g_pFence->SetEventOnCompletion(CURRENT_FENCE_VALUE, g_FenceEvent);
		WaitForSingleObject(g_FenceEvent, INFINITE);
	}

	g_CurrentFrameIndex = g_pSwapChain->GetCurrentBackBufferIndex();
}