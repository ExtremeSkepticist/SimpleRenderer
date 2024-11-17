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
#include "RenderingPipeline.h"

#include <DirectXTex.h>

#include "Camera.h"
#include "Common.h"
#include "Model.h"

using namespace Microsoft::WRL;

// D3D12 resources
const size_t g_FRAME_WIDTH = 800;
const size_t g_FRAME_HEIGHT = 600;
const size_t g_FRAME_COUNT = 2;
size_t g_CurrentFrameIndex = 0;
CD3DX12_VIEWPORT g_Viewport(0.0f, 0.0f, static_cast<float>(g_FRAME_WIDTH), static_cast<float>(g_FRAME_HEIGHT));
CD3DX12_RECT g_ScissorRect(0, 0, static_cast<LONG>(g_FRAME_WIDTH), static_cast<LONG>(g_FRAME_HEIGHT));
ComPtr<IDXGISwapChain3> g_pSwapChain = nullptr;
ComPtr<ID3D12Resource> g_pRenderTargets[g_FRAME_COUNT];
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
const XMFLOAT3 g_MODEL_WORLD_POSITION(0, 4, 2);

void WaitForPreviousFrame();

void InitGraphicsPipeline(unique_ptr<sr::platform::PlatformWindow>& platformWindow, unique_ptr<sr::rendering::RenderingPipeline>& renderingPipeline, HINSTANCE hInstance, int nCmdShow);
void LoadResources(gsl::not_null<gsl::czstring> modelPath, unique_ptr<sr::rendering::Model>& model, sr::rendering::RenderingPipeline& renderingPipeline);
void Update();
void Render(sr::rendering::RenderingPipeline& renderingPipeline);

// TODO: To be removed
sr::rendering::ShaderResourceDescriptorHeap* g_shaderResourceDescriptorHeap = nullptr;
sr::rendering::GraphicsPipelineState* g_graphicsPipelineState = nullptr;
sr::rendering::RootSignature* g_rootSignature = nullptr;

// TODO: deprecate d3dx12 and implement its own initialization
int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	unique_ptr<sr::platform::PlatformWindow> platformWindow = nullptr;
	unique_ptr<sr::rendering::RenderingPipeline> renderingPipeline = nullptr;

	InitGraphicsPipeline(platformWindow, renderingPipeline, hInstance, nCmdShow);

	gsl::czstring modelPath = "resources\\DamagedHelmet.gltf";
	unique_ptr<sr::rendering::Model> model = nullptr;
	LoadResources(modelPath, model, *renderingPipeline);

	while (true) {
		Render(*renderingPipeline);
	}

	return 0;
}

// 1. main: Init() -> LoadResources() -> Update() & Render()
// 2. Init(): InitPlatformWindow() -> InitPipeline()
// 3. LoadResource(): Load model -> Load Texture -> Load Shader
// RenderingPipeline class: Platform Window + GraphicsInfrastructure + ...
// RenderResources: Texture + Shader + ...
// TODO: Encapsulate all directx 12, WinApi logic inside a wrapper class
void InitGraphicsPipeline(unique_ptr<sr::platform::PlatformWindow>& platformWindow, unique_ptr<sr::rendering::RenderingPipeline>& renderingPipeline, HINSTANCE hInstance, int nCmdShow)
{
	Expects(platformWindow == nullptr);
	Expects(renderingPipeline == nullptr);
	Expects(hInstance != nullptr);

	gsl::not_null<gsl::cwzstring> profilerPath = L"C:\\Program Files\\Microsoft PIX\\2405.15.002-OneBranch_release\\WinPixGpuCapturer.dll";
	LoadLibrary(profilerPath);

#pragma region Init Platform
	const HRESULT isComInitialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	assert(SUCCEEDED(isComInitialized));

	constexpr uint16_t frameWidth = 800;
	constexpr uint16_t frameHeight = 600;
	gsl::not_null<gsl::cwzstring> windowName = L"SimpleRenderer";
	platformWindow = make_unique<sr::platform::PlatformWindow>(frameWidth, frameHeight, windowName, hInstance, nCmdShow);
#pragma endregion

// TODO: Rendering Pipeline Class that contains below
#pragma region Init Rendering Pipeline
	constexpr uint32_t swapChainBufferNum = 2;
	constexpr uint32_t infrastructureFlag = 0;
	renderingPipeline = make_unique<sr::rendering::RenderingPipeline>(*platformWindow, swapChainBufferNum, infrastructureFlag);

	const sr::rendering::GraphicsDevice& graphicsDevice = renderingPipeline->GetGraphicsDevice();
	const sr::rendering::SwapChain& swapChain = renderingPipeline->GetSwapChain();

	// Create RenderTargetHeap
	sr::rendering::RenderTargetDescriptorHeap renderTargetDescriptorHeap(swapChainBufferNum, swapChain, graphicsDevice);
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

		g_rootSignature = &rootSignature;
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

		g_graphicsPipelineState = &graphicsPipelineState;
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

	Ensures(platformWindow != nullptr);
	Ensures(renderingPipeline != nullptr);
}

void Render(sr::rendering::RenderingPipeline& renderingPipeline)
{
	sr::rendering::GraphicsCommandList& graphicsCommandList = renderingPipeline.GetGraphicsCommandList();

	graphicsCommandList.ResetCommandList();
	graphicsCommandList.SetPipelineState(*g_graphicsPipelineState);
	graphicsCommandList.SetGraphicsRootSignature(*g_rootSignature);

	ID3D12DescriptorHeap* pArrayDescriptorHeaps[] = { g_pDescriptorHeap.Get() };
	g_pCmdList->SetDescriptorHeaps(_countof(pArrayDescriptorHeaps), pArrayDescriptorHeaps);

	g_pCmdList->SetGraphicsRootDescriptorTable(0, g_pDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	g_pCmdList->RSSetViewports(1, &g_Viewport);
	g_pCmdList->RSSetScissorRects(1, &g_ScissorRect);
	
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(g_pRenderTargets[g_CurrentFrameIndex].Get()
		, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	g_pCmdList->ResourceBarrier(1, &resourceBarrier);
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(g_pRenderTargetViewHeap->GetCPUDescriptorHandleForHeapStart(), g_CurrentFrameIndex, renderingPipeline->GetGraphicsDevice()->GetD3d12Device()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
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
			g_pCmdList->DrawIndexedInstanced(meshes[i]->GetIndicesNum(), 1, 0, 0, 0);
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

void LoadResources(gsl::not_null<gsl::czstring> modelPath, unique_ptr<sr::rendering::Model>& model, sr::rendering::RenderingPipeline& renderingPipeline)
{
	Expects(model == nullptr);

	Assimp::Importer importer;
	constexpr uint32_t importFlag = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	const aiScene* scene = importer.ReadFile(modelPath, importFlag);
	assert(scene != nullptr);

	// TODO: Implement recursive initialization when needed
	model = make_unique<sr::rendering::Model>(*scene, g_MODEL_WORLD_POSITION, renderingPipeline.GetGraphicsDevice());

	WaitForPreviousFrame();

#pragma region Create Albedo Texture
	ScratchImage scratchImage = {};
	const HRESULT hasLoaded = LoadFromDDSFile(L"resources\\albedo.dds", DDS_FLAGS_NONE, nullptr, scratchImage);
	assert(SUCCEEDED(hasLoaded));

	const sr::rendering::GraphicsDevice& graphicsDevice = renderingPipeline.GetGraphicsDevice();
	sr::rendering::Texture albedoTexture(gsl::make_not_null<gsl::cwzstring>(L"albedo"), make_unique<ScratchImage>(std::move(scratchImage)), graphicsDevice);
	graphicsDevice.CreateShaderResourceView(albedoTexture, *g_shaderResourceDescriptorHeap, 1);

	sr::rendering::GraphicsCommandList& graphicsCommandList = renderingPipeline.GetGraphicsCommandList();
	graphicsCommandList.ResetCommandList();
	graphicsCommandList.RecordCommandCopyResource(albedoTexture.GetDefaultHeapResource(), albedoTexture.GetUploadHeapResource(), albedoTexture.GetSubResourceData());
	graphicsCommandList.FinishRecordingCommands();
	renderingPipeline.GetGraphicsCommandQueue().ExecuteCommandList(graphicsCommandList);

	WaitForPreviousFrame();
#pragma endregion

	Ensures(model != nullptr);
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