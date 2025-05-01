#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#include "ConstBuffer.h"
#include "CommandQueue.h"
#include "DescriptorRange.h"
#include "CommandList.h"
#include "GraphicsDevice.h"
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
#include "FrameResource.h"
#include "DescriptorHandle.h"
#include "LightSource.h"

#include <DirectXTex.h>

#include "Camera.h"
#include "Common.h"
#include "Model.h"

void InitGraphicsPipeline(unique_ptr<sr::platform::PlatformWindow>& platformWindow, unique_ptr<sr::rendering::RenderingPipeline>& renderingPipeline, int nCmdShow);
void LoadResources(gsl::not_null<gsl::czstring> modelPath, unique_ptr<sr::rendering::Model>& model
	, sr::rendering::RenderingPipeline& renderingPipeline
	, unique_ptr<sr::rendering::FrameResource>& frameResource
	, unique_ptr<sr::rendering::Texture>& albedo
	, unique_ptr<sr::rendering::Texture>& normal
	, unique_ptr<sr::rendering::ConstBuffer>& constBuffer);
void Update(sr::rendering::FrameResource& frameResource);
void Render(sr::rendering::RenderingPipeline& renderingPipeline);

// TODO: deprecate d3dx12 and implement its own initialization
int CALLBACK wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	unique_ptr<sr::platform::PlatformWindow> platformWindow = nullptr;
	unique_ptr<sr::rendering::RenderingPipeline> renderingPipeline = nullptr;

	InitGraphicsPipeline(platformWindow, renderingPipeline, nCmdShow);

	gsl::czstring modelPath = "resources\\DamagedHelmet.gltf";
	unique_ptr<sr::rendering::Model> model = nullptr;
	unique_ptr<sr::rendering::FrameResource> frameResource = nullptr;
	unique_ptr<sr::rendering::Texture> albedo = nullptr;
	unique_ptr<sr::rendering::Texture> normal = nullptr;
	unique_ptr<sr::rendering::ConstBuffer> constBuffer = nullptr;
	LoadResources(modelPath, model, *renderingPipeline, frameResource, albedo, normal, constBuffer);

	while (true) {
		renderingPipeline->Render(*frameResource);
	}

	return 0;
}

// TODO: Shouldn't GraphicsDevice class just create the entire object, instead of only d3d12 interface?
// e.g. initialize and return RootSignature object, instead of ID3D12RootSignature only
void InitGraphicsPipeline(unique_ptr<sr::platform::PlatformWindow>& platformWindow, unique_ptr<sr::rendering::RenderingPipeline>& renderingPipeline, int nCmdShow)
{
	Expects(platformWindow == nullptr);
	Expects(renderingPipeline == nullptr);

	gsl::not_null<gsl::cwzstring> profilerPath = L"C:\\Program Files\\Microsoft PIX\\2405.15.002-OneBranch_release\\WinPixGpuCapturer.dll";
	LoadLibrary(profilerPath);

	// Init Platform
	const HRESULT isComInitialized = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	assert(SUCCEEDED(isComInitialized));

	constexpr uint16_t frameWidth = 800;
	constexpr uint16_t frameHeight = 600;
	gsl::not_null<gsl::cwzstring> windowName = L"SimpleRenderer";
	platformWindow = make_unique<sr::platform::PlatformWindow>(frameWidth, frameHeight, windowName, nCmdShow);

	// Init graphics pipeline
	constexpr uint32_t swapChainBufferNum = 2; // TODO: change to 3
	constexpr uint32_t infrastructureFlag = 0;
	renderingPipeline = make_unique<sr::rendering::RenderingPipeline>(*platformWindow, swapChainBufferNum);

	Ensures(platformWindow != nullptr);
	Ensures(renderingPipeline != nullptr);
}

// TODO: copy from cpuFrameResource to gpuFrameResource. For now, use only one FrameResource for simplicity
// Create pipeline state object (a part of resource as well)
// Load textures, create & bind resources to descriptor heap
void LoadResources(gsl::not_null<gsl::czstring> modelPath, unique_ptr<sr::rendering::Model>& model
	, sr::rendering::RenderingPipeline& renderingPipeline
	, unique_ptr<sr::rendering::FrameResource>& frameResource
	, unique_ptr<sr::rendering::Texture>& albedo, unique_ptr<sr::rendering::Texture>& normal
	, unique_ptr<sr::rendering::ConstBuffer>& constBuffer)
{
	const sr::rendering::GraphicsDevice& device = renderingPipeline.GetGraphicsDevice();
	frameResource = make_unique<sr::rendering::FrameResource>(device);
	
	// Import Model
	Assimp::Importer importer;
	constexpr uint32_t importFlag = aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace;
	const aiScene* scene = importer.ReadFile(modelPath, importFlag);
	assert(scene != nullptr);

	constexpr XMFLOAT3 modelWorldPosition(0, 4, 2);
	model = make_unique<sr::rendering::Model>(*scene, modelWorldPosition, device); 	// TODO: Implement recursive initialization when needed
	frameResource->setModel(model.get());

	// Create render target view
	sr::rendering::SwapChain& swapChain = renderingPipeline.GetSwapChain();
	sr::rendering::RenderTargetDescriptorHeap& renderTargetDescriptorHeap = frameResource->GetRenderTargetDescriptorHeap();
	for (gsl::index i = 0; i < swapChain.GetBufferCount(); ++i)
	{
		sr::rendering::DescriptorHandle handle = renderTargetDescriptorHeap.GetHeapStartHandle();
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

	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(modelWorldPosition.x, modelWorldPosition.y, modelWorldPosition.z));
	worldMatrix = XMMatrixTranspose(worldMatrix);

	const XMMATRIX viewMatrix = XMMatrixTranspose(sceneCamera.GetViewLookAtMatrixLh(cameraWorldPosition, modelWorldPosition, worldUpDirection));
	const XMMATRIX projectionMatrix = XMMatrixTranspose(sceneCamera.GetProjectionMatrixLh());
	const XMMATRIX unused = {};
	
	//constexpr XMFLOAT3 pointLightSourceWorldPosition(modelWorldPosition.x, modelWorldPosition.y + 3, modelWorldPosition.z);
	// TODO: Store the max light intensity somewhere
	// const sr::rendering::PointLight lightSource = { XMFLOAT3(1, 1, 1), 100, pointLightSourceWorldPosition };

	const sr::rendering::CpuConstBufferData mappableConstBuffer{ worldMatrix, viewMatrix, projectionMatrix, unused };
	constBuffer = make_unique<sr::rendering::ConstBuffer>(L"Coord Transform Const buffer", mappableConstBuffer, device);
	
	sr::rendering::DescriptorHandle descriptorHandle = frameResource->GetShaderResourceDescriptorHeap().GetHeapStartHandle();
	device.CreateConstBufferView(*constBuffer, descriptorHandle);

	// Create and bind albedo texture
	ScratchImage scratchImage = {};
	// Pre-converted .dds file by texconv.exe
	// TODO: automate the conversion process somehow
	HRESULT hasLoaded = LoadFromDDSFile(L"resources\\albedo.dds", DDS_FLAGS_NONE, nullptr, scratchImage);
	assert(SUCCEEDED(hasLoaded));

	albedo = make_unique<sr::rendering::Texture>(gsl::make_not_null<gsl::cwzstring>(L"albedo"), make_unique<ScratchImage>(std::move(scratchImage)), device);
	descriptorHandle.cpuHandle.ptr += device.GetShaderResourceDescriptorHeapHandleIncrementSize();
	device.CreateShaderResourceView(*albedo, descriptorHandle);

	sr::rendering::CommandList& graphicsCommandList = renderingPipeline.GetGraphicsCommandList();
	graphicsCommandList.ResetCommandList();
	graphicsCommandList.RecordCommandCopyResource(albedo->GetDefaultHeapResource(), albedo->GetUploadHeapResource(), albedo->GetSubResourceData());
	graphicsCommandList.FinishRecordingCommands();
	renderingPipeline.GetGraphicsCommandQueue().ExecuteCommandList(graphicsCommandList);

	renderingPipeline.WaitForPreviousFrame();

	// Create and bind Normal map
	ScratchImage normalScratchImage = {};
	hasLoaded = LoadFromDDSFile(L"resources\\normal.dds", DDS_FLAGS_NONE, nullptr, normalScratchImage);
	assert(SUCCEEDED(hasLoaded));
	
	normal = make_unique<sr::rendering::Texture>(gsl::make_not_null<gsl::cwzstring>(L"normal")
		, make_unique<ScratchImage>(std::move(normalScratchImage)), device);
	descriptorHandle.cpuHandle.ptr += device.GetShaderResourceDescriptorHeapHandleIncrementSize();
	device.CreateShaderResourceView(*normal, descriptorHandle);

	graphicsCommandList.ResetCommandList();
	graphicsCommandList.RecordCommandCopyResource(normal->GetDefaultHeapResource(),
		normal->GetUploadHeapResource(), normal->GetSubResourceData());
	graphicsCommandList.FinishRecordingCommands();
	renderingPipeline.GetGraphicsCommandQueue().ExecuteCommandList(graphicsCommandList);

	Ensures(model != nullptr);
}