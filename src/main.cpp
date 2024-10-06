#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <Windows.h>
#include <DirectXMath.h>

#include "PlatformWindow.h"

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

HINSTANCE g_hInstance = nullptr;
HWND g_hWnd = nullptr;

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
const float g_FOV_ANGLE = 45;
const float g_NEAR_Z_COORD = 0.01;
const float g_FAR_Z_COORD = 100;
Camera g_Camera(g_FOV_ANGLE, g_FRAME_WIDTH / g_FRAME_HEIGHT, g_NEAR_Z_COORD, g_FAR_Z_COORD, g_CAMERA_WORLD_POSITION);

ConstBuffer g_ConstBufferData;
UINT8* g_pConstBufferMapped = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LoadPipeline();
void LoadRenderResources();
void WaitForPreviousFrame();

void Render();

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	HRESULT isComInitialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	if (FAILED(isComInitialized)) {
		assert(false, "Failed to initialie COM - error code: %ld", HRESULT_CODE(isComInitialized));
		return 1;
	}

	LoadLibrary(L"C:\\Program Files\\Microsoft PIX\\2405.15.002-OneBranch_release\\WinPixGpuCapturer.dll");
#pragma region Initialize Window
	constexpr uint16_t frameWidth = 800;
	PlatformWindow platformWindow();

	WNDCLASSEX windowClassEx = { 0, };
	windowClassEx.cbSize = sizeof(WNDCLASSEX);
	windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
	windowClassEx.lpfnWndProc = WndProc;
	windowClassEx.hInstance = hInstance;
	windowClassEx.lpszClassName = L"SimpleRenderer";
	
	ATOM result = RegisterClassEx(&windowClassEx);
	assert(result != FALSE);

	g_hInstance = hInstance;
	RECT rec = { 0, 0, g_FRAME_WIDTH, g_FRAME_HEIGHT };
	AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd = CreateWindow(L"SimpleRenderer", nullptr
		, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
		, CW_USEDEFAULT, CW_USEDEFAULT, rec.right - rec.left, rec.bottom - rec.top
		, nullptr, nullptr, hInstance, nullptr);
	assert(g_hWnd != nullptr);

#pragma endregion

	ShowWindow(g_hWnd, nCmdShow);
	LoadPipeline();

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

void LoadPipeline()
{
#pragma region Create Device
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory6> pDxgiFactory;
	HRESULT result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&pDxgiFactory));
	
	assert(pDxgiFactory != nullptr);

	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; ; ++adapterIndex)
	{
		pDxgiFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter));

		DXGI_ADAPTER_DESC1 desc = { 0, };
		adapter->GetDesc1(&desc);

		if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_pDevice))))
		{
			break;
		}
	}
	assert(g_pDevice != nullptr);

#pragma endregion

#pragma region Create Command Queue
	// Create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	g_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_pCmdQueue));
	assert(g_pCmdQueue != nullptr);
#pragma endregion

#pragma region Create SwapChain
	// TODO: Move all Window/SwapChain related things into a new class
	// Create SwapChain
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0, };
	swapChainDesc.BufferCount = g_FRAME_COUNT;
	swapChainDesc.Width = g_FRAME_WIDTH;
	swapChainDesc.Height = g_FRAME_HEIGHT;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;
	
	ComPtr<IDXGISwapChain1> pTempSwapChain = nullptr;
	pDxgiFactory->CreateSwapChainForHwnd(g_pCmdQueue.Get(), g_hWnd, &swapChainDesc, nullptr, nullptr, &pTempSwapChain);
	assert(pTempSwapChain != nullptr);

	pDxgiFactory->MakeWindowAssociation(g_hWnd, DXGI_MWA_NO_ALT_ENTER);
	pTempSwapChain.As(&g_pSwapChain);
	g_CurrentFrameIndex = g_pSwapChain->GetCurrentBackBufferIndex();
#pragma endregion

#pragma region Create RenderTarget View
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = g_FRAME_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	g_pDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&g_pRenderTargetViewHeap));
	assert(g_pRenderTargetViewHeap != nullptr);

	// Bind RenderTarget Resource to descriptor heap
	CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetHandle(g_pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());
	for (size_t i = 0; i < g_FRAME_COUNT; ++i)
	{
		g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&g_pRenderTargets[i]));
		g_pDevice->CreateRenderTargetView(g_pRenderTargets[i].Get(), nullptr, renderTargetHandle);
		renderTargetHandle.Offset(1, g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
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
#pragma region Initialize Root Signature
	const size_t DESCRIPTOR_RANGE_NUM = 2;
	CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[DESCRIPTOR_RANGE_NUM] = {};
	descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	descriptorRanges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);

	const size_t ROOT_PARAMETER_NUM = 1;
	CD3DX12_ROOT_PARAMETER1 rootParameters[ROOT_PARAMETER_NUM] = {};
	rootParameters[0].InitAsDescriptorTable(DESCRIPTOR_RANGE_NUM, descriptorRanges);
	
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	{
		samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		samplerDesc.MaxAnisotropy = 16;
		samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
		samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	}

	D3D12_ROOT_SIGNATURE_FLAGS roFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC roDesc = {};
	roDesc.Init_1_1(ROOT_PARAMETER_NUM, rootParameters, 1, &samplerDesc, roFlags);
	
	ComPtr<ID3DBlob> pSerializedSignature = nullptr;
	ComPtr<ID3DBlob> pError= nullptr;
	D3DX12SerializeVersionedRootSignature(&roDesc, D3D_ROOT_SIGNATURE_VERSION_1_1, &pSerializedSignature, &pError);
	
	assert(pSerializedSignature != nullptr);

	g_pDevice->CreateRootSignature(0, pSerializedSignature->GetBufferPointer(), pSerializedSignature->GetBufferSize(), IID_PPV_ARGS(&g_pRootSignature));
	assert(g_pRootSignature != nullptr);

#pragma endregion

#pragma region Initialize shader
	// TODO: Use batch script instead to compile shaders
	// TODO: Implement changing shaders upon using different effects
	ComPtr<ID3DBlob> pVertexShader;
	ComPtr<ID3DBlob> pPixelShader;

	D3DReadFileToBlob(L"shaders\\BasicVertexShader.cso", &pVertexShader);
	assert(pVertexShader != nullptr);
	D3DReadFileToBlob(L"shaders\\BasicPixelShader.cso", &pPixelShader);
	assert(pPixelShader != nullptr);
#pragma endregion

#pragma region Initialize PSO
	// Initialize PSO
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		, {"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		, {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		, {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		, {"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0, };
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) }; // TODO: make Mesh class return Vertex-struct-related data
	psoDesc.pRootSignature = g_pRootSignature.Get();
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(pVertexShader.Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pPixelShader.Get());
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

#pragma region Create Fence
	g_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence));
	g_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	// Wait for the command list to execute; we are reusing the same command 
	// list in our main loop but for now, we just want to wait for setup to 
	// complete before continuing.
	WaitForPreviousFrame();
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