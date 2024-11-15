#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include "ConstBuffer.h"
#include "DescriptorRange.h"
#include "GraphicsCommandList.h"
#include "GraphicsCommandQueue.h"
#include "GraphicsDevice.h"
#include "GraphicsInfrastructure.h"
#include "GraphicsPipelineState.h"
#include "GpuResource.h"
#include "Fence.h"
#include "InputElementLayout.h"
#include "PlatformWindow.h"
#include "RenderTargetDescriptorHeap.h"
#include "RootParameter.h"
#include "RootSignature.h"
#include "ShaderResourceDescriptorHeap.h"
#include "Shader.h"
#include "SwapChain.h"
#include "Texture.h"

#include <DirectXTex.h>

#include "Camera.h"
#include "Common.h"
#include "Model.h"

using namespace Microsoft::WRL;

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
sr::rendering::Model* g_pModel = nullptr;
const XMFLOAT3 g_MODEL_WORLD_POSITION(0, 4, 2);

void LoadRenderResources();
void WaitForPreviousFrame();

void Init(HINSTANCE hInstance, int nCmdShow);
void Update();
void Render();

// TODO: To be removed
sr::rendering::GraphicsDevice* g_graphicsDevicePtr = nullptr;
sr::rendering::GraphicsCommandList* g_graphicsCommandListPtr = nullptr;
sr::rendering::GraphicsCommandQueue* g_graphicsCommandQueue = nullptr;
sr::rendering::ShaderResourceDescriptorHeap* g_shaderResourceDescriptorHeap = nullptr;

// TODO: deprecate d3dx12 and implement its own initialization
int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	Init(hInstance, nCmdShow);

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("resources\\DamagedHelmet.gltf", aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);
	assert(scene != nullptr);

	// TODO: Implement recursive initialization when needed
	sr::rendering::Model model(*scene, g_MODEL_WORLD_POSITION);
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

		g_graphicsDevicePtr = &graphicsDevice;
	}	
	
	sr::rendering::GraphicsCommandQueue graphicsCommandQueue(sr::rendering::GraphicsCommandQueueType::Graphics, graphicsDevice);
	{
		// TODO: to be removed
		g_pCmdQueue = graphicsCommandQueue.GetD3d12CommandQueue();
		assert(g_pCmdQueue != nullptr);

		g_graphicsCommandQueue = &graphicsCommandQueue;
	}

	sr::rendering::GraphicsCommandList graphicsCommandList(graphicsDevice);
	{
		g_pCmdAllocator = graphicsCommandList.GetD3d12CommandAllocatorOwner();
		assert(g_pCmdAllocator != nullptr);
		
		g_pCmdList = graphicsCommandList.GetD3d12GraphicsCommandListOwner();
		assert(g_pCmdList != nullptr);

		g_graphicsCommandListPtr = &graphicsCommandList;
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

	sr::rendering::RootSignature rootSignature(rootParameters, graphicsDevice);
	{
		// TODO: to be removed
		g_pRootSignature = rootSignature.GetD3d12RootSignatureOwner();
		assert(g_pRootSignature != nullptr);
	}
#pragma endregion

#pragma region Loading Graphics Resources

	// Initialize Shader
	//	- TODO: Use batch script instead to compile shaders
	//	- TODO: Implement changing shaders upon using different effects
	constexpr gsl::cwzstring vertexShaderPath = L"shaders\\BasicVertexShader.cso";
	const sr::rendering::Shader vertexShader(vertexShaderPath);
	constexpr gsl::cwzstring pixelShaderPath = L"shaders\\BasicPixelShader.cso";
	const sr::rendering::Shader pixelShader(pixelShaderPath);

	// Initialize input element layout
	//	- TODO: Could this be moved into mesh class or somewhere else?
	constexpr uint32_t inputSlot = 0;
	sr::rendering::InputElementLayout inputElementLayout(inputSlot);
	inputElementLayout.AddLayout("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	inputElementLayout.AddLayout("TEXCOORDS", 0, DXGI_FORMAT_R32G32_FLOAT);
	inputElementLayout.AddLayout("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	inputElementLayout.AddLayout("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT);
	inputElementLayout.AddLayout("BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT);

	sr::rendering::GraphicsPipelineState graphicsPipelineState(rootSignature, inputElementLayout, vertexShader, pixelShader, graphicsDevice);
	{
		// Todo: To be removed
		g_pPipelineStateObj = graphicsPipelineState.GetPipelineStateOwner();
		assert(g_pPipelineStateObj != nullptr);
	}

	// Create Shader Resource descriptor heap
	constexpr uint32_t numDescriptors = 2;
	sr::rendering::ShaderResourceDescriptorHeap shaderResourceDescriptorHeap(numDescriptors, graphicsDevice);
	{
		g_pDescriptorHeap = shaderResourceDescriptorHeap.GetDescriptorHeap();
		assert(g_pDescriptorHeap != nullptr);

		g_shaderResourceDescriptorHeap = &shaderResourceDescriptorHeap;
	}

	// Create Scene Camera
	constexpr XMFLOAT3 cameraWorldPosition(0, 6, 0);
	constexpr XMFLOAT3 worldUpDirection(0, 1, 0);
	constexpr float fovAngle = 45.0f;
	constexpr float nearZcoordinate = 0.01f;
	constexpr float farZcoordinate = 100.0f;
	const Camera sceneCamera(fovAngle, sr::platform::aspectRatio, nearZcoordinate, farZcoordinate, cameraWorldPosition);

	// Create const buffer
	XMMATRIX worldMatrix = XMMatrixMultiply(XMMatrixScaling(0.8f, 0.8f, 0.8f), XMMatrixRotationX(-0.8));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(-0.1));
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(g_MODEL_WORLD_POSITION.x, g_MODEL_WORLD_POSITION.y, g_MODEL_WORLD_POSITION.z));
	worldMatrix = XMMatrixTranspose(worldMatrix);

	const XMMATRIX viewMatrix = XMMatrixTranspose(sceneCamera.GetViewLookAtMatrixLh(cameraWorldPosition, g_MODEL_WORLD_POSITION, worldUpDirection));
	const XMMATRIX projectionMatrix = XMMatrixTranspose(sceneCamera.GetProjectionMatrixLh());
	const XMMATRIX unused = {};

	const sr::rendering::CpuConstBufferData mappableConstBuffer{ worldMatrix, viewMatrix, projectionMatrix, unused };
	sr::rendering::ConstBuffer constBuffer(L"Coord Transform Const buffer", mappableConstBuffer, graphicsDevice);
	constexpr uint32_t constBufferViewDescriptorIndex = 0;
	graphicsDevice.CreateConstBufferView(constBuffer, shaderResourceDescriptorHeap, constBufferViewDescriptorIndex);
	{
		g_pConstBuffer = constBuffer.GetGpuResource().GetD3d12ResourceOwner();
		assert(g_pConstBuffer != nullptr);
	}

	// Create fence
	sr::rendering::Fence fence(graphicsDevice);
	{
		g_pFence = fence.GetD3d12Fence();
		g_FenceEvent = fence.GetFenceEventOwner();

		assert(g_pFence != nullptr);
		assert(g_FenceEvent != nullptr);
	}

#pragma endregion
}

void Render()
{
	HRESULT result = g_pCmdAllocator->Reset();
	assert(SUCCEEDED(result));

	result = g_pCmdList->Reset(g_pCmdAllocator.Get(), g_pPipelineStateObj.Get());
	assert(SUCCEEDED(result));

	g_pCmdList->SetPipelineState(g_pPipelineStateObj.Get());
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
	// Wait for the command list to execute; we are reusing the same command 
	// list in our main loop but for now, we just want to wait for setup to 
	// complete before continuing.
	WaitForPreviousFrame();

#pragma region Create Albedo Texture
	ScratchImage scratchImage = {};
	const HRESULT hasLoaded = LoadFromDDSFile(L"resources\\albedo.dds", DDS_FLAGS_NONE, nullptr, scratchImage);
	assert(SUCCEEDED(hasLoaded));

	sr::rendering::Texture albedoTexture(gsl::make_not_null<gsl::cwzstring>(L"albedo"), make_unique<ScratchImage>(std::move(scratchImage)), *g_graphicsDevicePtr);
	g_graphicsDevicePtr->CreateShaderResourceView(albedoTexture, *g_shaderResourceDescriptorHeap, 1);

	g_graphicsCommandListPtr->ResetCommandList();
	g_graphicsCommandListPtr->RecordCommandCopyResource(albedoTexture.GetDefaultHeapResource(), albedoTexture.GetUploadHeapResource(), albedoTexture.GetSubResourceData());
	g_graphicsCommandListPtr->FinishRecordingCommands();
	g_graphicsCommandQueue->ExecuteCommandList(*g_graphicsCommandListPtr);

	WaitForPreviousFrame();
#pragma endregion

#pragma region Create Vertex & Index Buffer
	auto& meshes = g_pModel->GetMeshes();
	for (auto& mesh : meshes)
	{
		// TODO: Use DEFAULT heap instead
		const size_t VERTEX_BUFFER_SIZE = sizeof(sr::rendering::Mesh::Vertex) * mesh->GetVerticeNum();
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
		vertexBufferView.StrideInBytes = sizeof(sr::rendering::Mesh::Vertex);
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