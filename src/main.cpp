#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <d3d12.h>
#include <d3dx12_core.h>
#include <dxgi1_6.h>
#include <Windows.h>

#include "Common.h"
#include "Model.h"

using namespace Microsoft::WRL;

HINSTANCE g_hInstance = nullptr;
HWND g_hWnd = nullptr;
// vector<Vertex> g_Vertices;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void LoadPipeline(ID3D12Device** device);

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

	ComPtr<ID3D12Device> device = nullptr;
	LoadPipeline(&device);

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
			//device->CreateCommittedResource(
			//	&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD)
			//	, D3D12_HEAP_FLAG_NONE
			//	, &CD3DX12_RESOURCE_DESC::Buffer())

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
void LoadPipeline(ID3D12Device** device)
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

		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device))))
		{
			break;
		}
	}

	assert(device != nullptr);
}