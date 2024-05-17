#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <Windows.h>
#include "Common.h"
#include "Model.h"

using namespace Microsoft::WRL;

HINSTANCE g_hInstance = nullptr;
HWND g_hWnd = nullptr;

// D3D12 resources
ComPtr<ID3D12Device> g_pDevice = nullptr;
ComPtr<ID3D12CommandAllocator> g_pCmdAllocator = nullptr;
ComPtr<ID3D12CommandQueue> g_pCmdQueue = nullptr;
ComPtr<ID3D12RootSignature> g_pRootSignature = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LoadPipeline();
void LoadRenderResources();

void Render();

int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	WNDCLASSEX windowClassEx = { 0, };
	windowClassEx.cbSize = sizeof(WNDCLASSEX);
	windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
	windowClassEx.lpfnWndProc = WndProc;
	windowClassEx.hInstance = hInstance;
	windowClassEx.lpszClassName = L"SimpleRenderer";
	
	ATOM result = RegisterClassEx(&windowClassEx);
	assert(result != FALSE);

	g_hInstance = hInstance;
	RECT rec = { 0, 0, 800, 600 };
	AdjustWindowRect(&rec, WS_OVERLAPPEDWINDOW, FALSE);

	g_hWnd = CreateWindow(L"SimpleRenderer", nullptr
		, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX
		, CW_USEDEFAULT, CW_USEDEFAULT, rec.right - rec.left, rec.bottom - rec.top
		, nullptr, nullptr, hInstance, nullptr);
	assert(g_hWnd != nullptr);

	ShowWindow(g_hWnd, nCmdShow);

	LoadPipeline();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("resources\\DamagedHelmet.gltf", 0);
	assert(scene != nullptr);

	// TODO: Implement recursive initialization when needed
	Model model(*scene);

	// Create Vertex Buffer
	{
		auto& meshes = model.GetMeshes();
		for (auto& mesh : meshes)
		{
			// TODO: Use DEFAULT heap instead
			const size_t VERTEX_BUFFER_SIZE = sizeof(Mesh::Vertex) * mesh->GetVerticeNum();
			CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_UPLOAD);
			CD3DX12_RESOURCE_DESC vertexBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(VERTEX_BUFFER_SIZE);

			HRESULT IsVertexBufferCreated = g_pDevice->CreateCommittedResource(
				&heapProp
				, D3D12_HEAP_FLAG_NONE
				, &vertexBufferDesc
				, D3D12_RESOURCE_STATE_GENERIC_READ
				, nullptr
				, IID_PPV_ARGS(&mesh->GetVertexBuffer()));

			assert(SUCCEEDED(IsVertexBufferCreated));

			UINT8* ptrVertexDataMapped = nullptr;
			CD3DX12_RANGE readRange(0, 0);
			mesh->GetVertexBuffer()->Map(0, &readRange, reinterpret_cast<void**>(&ptrVertexDataMapped));
			{				
				memcpy(ptrVertexDataMapped, &mesh->GetVerticeData(), sizeof(Mesh::Vertex));
			}
			mesh->GetVertexBuffer()->Unmap(0, nullptr);

			D3D12_VERTEX_BUFFER_VIEW& vertexBufferView = mesh->GetVertexBufferView();
			vertexBufferView.BufferLocation = mesh->GetVertexBuffer()->GetGPUVirtualAddress();
			vertexBufferView.StrideInBytes = sizeof(Mesh::Vertex);
			vertexBufferView.SizeInBytes = VERTEX_BUFFER_SIZE;
		}
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

// TODO: Add upload heap for uploading vertex
void LoadPipeline()
{
	UINT dxgiFactoryFlags = 0;
#if defined(_DEBUG)
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
		dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
	}
#endif

	ComPtr<IDXGIFactory6> factory;
	HRESULT result = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory));
	
	assert(factory != nullptr);

	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; ; ++adapterIndex)
	{
		factory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_UNSPECIFIED, IID_PPV_ARGS(&adapter));

		DXGI_ADAPTER_DESC1 desc = { 0, };
		adapter->GetDesc1(&desc);

		if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE))
		{
			continue;
		}

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&g_pDevice))))
		{
			break;
		}
	}

	assert(g_pDevice != nullptr);

	// Create command queue & command list
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	g_pDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&g_pCmdQueue));
	assert(g_pCmdQueue != nullptr);

	g_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_pCmdAllocator));
	assert(g_pCmdAllocator != nullptr);
}

void Render()
{
	HRESULT result = g_pCmdAllocator->Reset();
	assert(SUCCEEDED(result));
}

// TODO
void LoadRenderResources()
{
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		, {"TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		, {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	
	// Initialize Root Signature
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

	// Initialize PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = { 0, };
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) }; // TODO: make Mesh class return Vertex-struct-related data
	psoDesc.pRootSignature = g_pRootSignature.Get();
}