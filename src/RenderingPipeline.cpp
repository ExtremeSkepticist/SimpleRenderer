#include "Fence.h"
#include "FrameResource.h"
#include "CommandList.h"
#include "CommandType.h"
#include "CommandQueue.h"
#include "GraphicsDevice.h"
#include "GraphicsPipelineState.h"
#include "PlatformWindow.h"
#include "RenderingPipeline.h"
#include "RenderTargetDescriptorHeap.h"
#include "RootSignature.h"
#include "ShaderResourceDescriptorHeap.h"
#include "SwapChain.h"
#include "Model.h"

namespace sr
{
	namespace rendering
	{
		// TODO: Separeate SwapChain and DescriptorHeap from Pipeline. They should be FrameResource, and pipeline should be just "tools"
		// TODO: Use Device to create d3d12 object, outside its constructor
		// TODO: investigate scene graph
		//  - Separate resources from DescriptorHeap. Resources should go into Input struct/class and get bound later.
		// TODO: Redesign the following:
		//	Consider separating command allocator from command list? Could just pass different allocators that are not in recording
		//	Refer to complete code; maybe have Gpu descriptor heap in RenderingPipeline class and copy from cpu descriptor heap in Update(
		RenderingPipeline::RenderingPipeline(platform::PlatformWindow& platformWindow, const uint32_t swapChainBufferNum) noexcept
			: mGraphicsDevice(make_unique<GraphicsDevice>())
			, mGraphicsCommandQueue(make_unique<CommandQueue>(CommandType::Graphics))
			, mGraphicsCommandList(make_unique<CommandList>(CommandType::Graphics))
			, mSwapChain(make_unique<SwapChain>(swapChainBufferNum, platformWindow))
			, mFence(make_unique<Fence>())
			// , mFrameResource(make_unique<FrameResource>())
		{
			mGraphicsDevice->CreateCommandQueue(*mGraphicsCommandQueue);
			mGraphicsDevice->CreateCommandList(*mGraphicsCommandList);
			mGraphicsDevice->CreateSwapChain(*mSwapChain, platformWindow, *mGraphicsCommandQueue);
			mGraphicsDevice->CreateFence(*mFence);
		}

		// TODO: Implement copy from CPU FrameResource to GPU FrameResource
	/*	void RenderingPipeline::Update(const FrameResource& frameResource)
		{

		}*/

		void RenderingPipeline::Render(FrameResource& frameResource)
		{
			setupDefaultCommands(frameResource);
		}

		void RenderingPipeline::setupDefaultCommands(FrameResource& frameResource)
		{
			sr::rendering::CommandList& graphicsCommandList = GetGraphicsCommandList();

			graphicsCommandList.ResetCommandList();
			graphicsCommandList.SetPipelineState(frameResource.GetPipelineState());
			graphicsCommandList.SetGraphicsRootSignature(frameResource.GetRootSignature());
			
			// TODO: Encapsulate all d3d12 functions into CommandList class

			// Bind SRV heap
			gsl::not_null<ID3D12GraphicsCommandList*> d3d12CommandList = graphicsCommandList.GetCommandList();
			gsl::not_null<ID3D12DescriptorHeap*> srvHeap = frameResource.GetShaderResourceDescriptorHeap().GetDescriptorHeap();
			ID3D12DescriptorHeap* arr[] = { srvHeap };
			d3d12CommandList->SetDescriptorHeaps(1, arr);
			d3d12CommandList->SetGraphicsRootDescriptorTable(0, srvHeap->GetGPUDescriptorHandleForHeapStart());

			// Bind Viewport & scissor rect
			const uint16_t frameWidth = mSwapChain->GetBufferWidth();
			const uint16_t frameHeight = mSwapChain->GetBufferHeight();
			CD3DX12_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(frameWidth), static_cast<float>(frameHeight));
			CD3DX12_RECT scissorRect(0, 0, static_cast<LONG>(frameWidth), static_cast<LONG>(frameHeight));
			d3d12CommandList->RSSetViewports(1, &viewport);
			d3d12CommandList->RSSetScissorRects(1, &scissorRect);

			// Transition RT state
			ID3D12Resource* backBuffer = nullptr;
			const uint32_t currentBackBufferIndex = mSwapChain->GetBackBufferIndex();
			mSwapChain->GetDxgiSwapChain()->GetBuffer(currentBackBufferIndex, IID_PPV_ARGS(&backBuffer));
			CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
				backBuffer
				, D3D12_RESOURCE_STATE_PRESENT
				, D3D12_RESOURCE_STATE_RENDER_TARGET
			);

			d3d12CommandList->ResourceBarrier(1, &resourceBarrier);
			{
				ID3D12DescriptorHeap* rtvHeap = frameResource.GetRenderTargetDescriptorHeap().GetDescriptorHeap();
				CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), currentBackBufferIndex, mGraphicsDevice->GetRenderTargetHeapHandleIncrementSize());
				d3d12CommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

				const FLOAT pClearingColor[] = { 0, 0, 0, 0 };
				d3d12CommandList->ClearRenderTargetView(rtvHandle, pClearingColor, 0, nullptr);
				d3d12CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

				gsl::not_null<Model*> model = frameResource.getModel();
				const size_t MESH_COUNT = model->GetMeshes().size();
				auto& meshes = model->GetMeshes();
				for (size_t i = 0; i < MESH_COUNT; ++i)
				{
					d3d12CommandList->IASetVertexBuffers(0, 1, &meshes[i]->GetVertexBufferView());
					d3d12CommandList->IASetIndexBuffer(&meshes[i]->GetIndexBufferView());
					d3d12CommandList->DrawIndexedInstanced(meshes[i]->GetIndicesNum(), 1, 0, 0, 0);
				}
			}
			resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			d3d12CommandList->ResourceBarrier(1, &resourceBarrier);

			d3d12CommandList->Close();

			ID3D12CommandList* cmdListArray[] = { d3d12CommandList };
			mGraphicsCommandQueue->GetD3d12CommandQueue()->ExecuteCommandLists(_countof(cmdListArray), cmdListArray);

			mSwapChain->GetDxgiSwapChain()->Present(1, 0);

			WaitForPreviousFrame();
		}

		//TODO: lOOK AT D3D12HellOFrameBuffering
		void RenderingPipeline::WaitForPreviousFrame() noexcept
		{
			uint64_t currentFenceValue = mFence->GetCurrentFenceValue() + 1;
			ID3D12Fence* d3d12Fence = mFence->GetD3d12Fence();

			// TODO: abstract this
			mGraphicsCommandQueue->GetD3d12CommandQueue()->Signal(d3d12Fence, currentFenceValue);

			// Wait until fence-signalling command is run by command queue
			if (mFence->GetCurrentFenceValue() < currentFenceValue)
			{
				// Wait until fence event is run, after fence reached the specified value
				HANDLE& event = mFence->GetFenceEventOwner();
				d3d12Fence->SetEventOnCompletion(currentFenceValue, event);
				WaitForSingleObject(event, INFINITE);
			}

			mSwapChain->UpdateCurrentBackBufferIndex();
		}
	}
}