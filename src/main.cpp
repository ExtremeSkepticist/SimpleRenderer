#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <Windows.h>
#include "Common.h"
#include "Model.h"

using namespace Microsoft::WRL;

//TODO: Texture -https://github.com/microsoft/DirectXTK12/blob/main/Src/WICTextureLoader.cpp#L642
// TODO: Fill this with transform matrix

#pragma pack(256)
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
ComPtr<ID3D12DescriptorHeap> g_pConstBufferViewHeap = nullptr;
ComPtr<ID3D12Resource> g_pConstBuffer = nullptr;
ComPtr<ID3D12CommandAllocator> g_pCmdAllocator = nullptr;
ComPtr<ID3D12GraphicsCommandList> g_pCmdList = nullptr;
ComPtr<ID3D12CommandQueue> g_pCmdQueue = nullptr;
ComPtr<ID3D12RootSignature> g_pRootSignature = nullptr;
ComPtr<ID3D12PipelineState> g_pPipelineStateObj = nullptr;
ComPtr<ID3D12Fence> g_pFence = nullptr;
UINT64 g_FenceValue = 1;
HANDLE g_FenceEvent = nullptr;

Model* g_pModel = nullptr;

// Camera related variables
XMFLOAT3 g_WorldCameraPosition(0, 3, 0);
XMFLOAT3 g_WorldCameraLookToDirection(0, 0, 1);
XMFLOAT3 g_WorldUpDirection(0, 1, 0);
float g_FovAngle = 45;
const float g_NEAR_Z_COORD = 0.01;
const float g_FAR_Z_COORD = 1;

ConstBuffer g_ConstBufferData;
UINT8* g_pConstBufferMapped = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LoadPipeline();
void LoadRenderResources();
void WaitForPreviousFrame();

void Render();

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	LoadLibrary(L"C:\\Program Files\\Microsoft PIX\\2405.15.002-OneBranch_release\\WinPixGpuCapturer.dll");
#pragma region Initialize Window
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
	const aiScene* scene = importer.ReadFile("resources\\DamagedHelmet.gltf", 0);
	assert(scene != nullptr);

	// TODO: Implement recursive initialization when needed
	Model model(*scene);
	g_pModel = &model;

	LoadRenderResources();
	
	while (true) {
		Render();
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct = { 0, };
	HDC hDevice = { 0, };

	switch (message)
	{
	case WM_PAINT:
		hDevice = BeginPaint(hWnd, &paintStruct);
		EndPaint(hWnd, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
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

	ID3D12DescriptorHeap* pArrayDescriptorHeaps[] = { g_pConstBufferViewHeap.Get() };
	g_pCmdList->SetDescriptorHeaps(_countof(pArrayDescriptorHeaps), pArrayDescriptorHeaps);

	// TODO: Set Descriptor table with shader resources
	g_pCmdList->SetGraphicsRootDescriptorTable(0, g_pConstBufferViewHeap->GetGPUDescriptorHandleForHeapStart());
	g_pCmdList->RSSetViewports(1, &g_Viewport);
	g_pCmdList->RSSetScissorRects(1, &g_ScissorRect);


	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(g_pRenderTargets[g_CurrentFrameIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	g_pCmdList->ResourceBarrier(1, &resourceBarrier);
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), g_CurrentFrameIndex, g_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		g_pCmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

		const FLOAT pClearingColor[] = { 1, 0, 0, 0 };
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
	const size_t DESCRIPTOR_RANGE_NUM = 1;
	CD3DX12_DESCRIPTOR_RANGE1 descriptorRanges[DESCRIPTOR_RANGE_NUM] = {};
	descriptorRanges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, DESCRIPTOR_RANGE_NUM, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
	
	const size_t ROOT_PARAMETER_NUM = 1;
	CD3DX12_ROOT_PARAMETER1 rootParameters[ROOT_PARAMETER_NUM] = {};
	rootParameters[0].InitAsDescriptorTable(ROOT_PARAMETER_NUM, &descriptorRanges[0], D3D12_SHADER_VISIBILITY_VERTEX);

	D3D12_ROOT_SIGNATURE_FLAGS roFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC roDesc = {};
	roDesc.Init_1_1(ROOT_PARAMETER_NUM, rootParameters, 0, nullptr, roFlags);
	
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

#pragma region Create ConstBuffer
	// Create ConstBufferView Heap
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	g_pDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&g_pConstBufferViewHeap));
	assert(g_pConstBufferViewHeap != nullptr);

	// Create GPU resource for Const Buffer
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

	D3D12_CONSTANT_BUFFER_VIEW_DESC constBufferDesc = { 0, };
	constBufferDesc.BufferLocation = g_pConstBuffer->GetGPUVirtualAddress();
	constBufferDesc.SizeInBytes = CONST_BUFFER_SIZE;
	g_pDevice->CreateConstantBufferView(&constBufferDesc, g_pConstBufferViewHeap->GetCPUDescriptorHandleForHeapStart());

	g_ConstBufferData.mWorldMatrix = XMMatrixTranspose(XMMatrixIdentity());
	g_ConstBufferData.mViewMatrix = XMMatrixTranspose(XMMatrixLookToLH(XMLoadFloat3(&g_WorldCameraPosition), XMLoadFloat3(&g_WorldCameraLookToDirection), XMLoadFloat3(&g_WorldUpDirection)));
	g_ConstBufferData.mProjectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(g_FovAngle, g_FRAME_WIDTH / g_FRAME_HEIGHT, g_NEAR_Z_COORD, g_FAR_Z_COORD));

	// Map and initialize the constant buffer. We don't unmap this until the
	// app closes. Keeping things mapped for the lifetime of the resource is okay.
	// TODO: how do map() and unmap() work internally?
	CD3DX12_RANGE constBufferReadRange(0, 0);
	g_pConstBuffer->Map(0, &constBufferReadRange, reinterpret_cast<void**>(&g_pConstBufferMapped));
	memcpy(g_pConstBufferMapped, &g_ConstBufferData, sizeof(g_ConstBufferData));

#pragma endregion

#pragma region Create Fence
	g_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_pFence));
	g_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

	// Wait for the command list to execute; we are reusing the same command 
	// list in our main loop but for now, we just want to wait for setup to 
	// complete before continuing.
	WaitForPreviousFrame();

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