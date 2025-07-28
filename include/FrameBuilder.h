#pragma once

#include <d3d12.h>

#include "Common.h"
#include "Mesh.h"

namespace sr
{
	namespace rendering
	{
		struct CpuConstBufferData;
		
		class ConstBuffer;
		class GraphicsDevice;
		class GraphicsPipelineState;
		class RenderTargetDescriptorHeap;
		class RenderingPipeline;
		class RootSignature;
		class Scene;
		class ShaderResourceDescriptorHeap;
		class Mesh;
		class GpuResource;

		struct DescriptorHandle;

		class FrameBuilder
		{
		public:
			FrameBuilder(RenderingPipeline& pipeline, Scene& scene);
			RenderTargetDescriptorHeap& GetRenderTargetDescriptorHeap();
			ShaderResourceDescriptorHeap& GetShaderResourceDescriptorHeap();
			GraphicsPipelineState& GetPipelineState();
			RootSignature& GetRootSignature();
			~FrameBuilder();

			void Update(gsl::not_null<Scene*> scene);

			// TODO: abstract this
			ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;
			ComPtr<ID3D12Resource> mDsvBuffer = nullptr;
			
			inline void SetMesh(gsl::not_null<Mesh*> mesh, const GraphicsDevice& device)
			{
				mIsDirty = true;

				mMesh = mesh.get();
				mIndexNum = mMesh->GetIndicesNum();
				createMeshResources(device);
			}

			inline D3D12_VERTEX_BUFFER_VIEW& GetVertexView() noexcept
			{
				return mVertexBufferView;
			}
			inline D3D12_INDEX_BUFFER_VIEW& GetIndexView() noexcept
			{
				return mIndexBufferView;
			}
			inline size_t GetIndexNum() const noexcept
			{
				return mIndexNum;
			}

		private:
			void createDefaultResources(RenderingPipeline& pipeline, Scene& scene);
			void createDefaultRootSignature(const GraphicsDevice& device);
			void createDefaultPipelineState(const GraphicsDevice& device);
			void createMeshResources(const GraphicsDevice& device) noexcept;

			void recordCommands();

			unique_ptr<RootSignature> mRootSignature = nullptr;
			unique_ptr<GraphicsPipelineState> mPipelineState = nullptr;
			unique_ptr<RenderTargetDescriptorHeap> mRenderTargetDescriptorHeap = nullptr;
			unique_ptr<ShaderResourceDescriptorHeap> mShaderResourceDescriptorHeap = nullptr;
			unique_ptr<ConstBuffer> mConstBuffer = nullptr;
			unique_ptr<CpuConstBufferData> mCpuConstBufferData = nullptr;
			
			const Mesh* mMesh = nullptr;
			unique_ptr<GpuResource> mVertexBufferResource = nullptr;
			unique_ptr<GpuResource> mIndexBufferResource = nullptr;
			D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
			D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
			size_t mIndexNum = 0;

			bool mIsDirty = false;
		};
	}
}
