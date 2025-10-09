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
		class TextureGroup;

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

			void Build(RenderableNode& node, Camera& camera, GraphicsDevice& device) noexcept;

			// TODO: abstract this
			ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;
			ComPtr<ID3D12Resource> mDsvBuffer = nullptr;

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
			void createDefaultResources(RenderingPipeline& pipeline) noexcept;
			void createDefaultRootSignature(const GraphicsDevice& device);
			void createDefaultPipelineState(const GraphicsDevice& device);
			void createMeshResources(const GraphicsDevice& device) noexcept;
			void bindSrvResources(gsl::not_null<TextureGroup*> textureGroup, const GraphicsDevice& device) noexcept;

			unique_ptr<RootSignature> mRootSignature = nullptr;
			unique_ptr<GraphicsPipelineState> mPipelineState = nullptr;
			unique_ptr<RenderTargetDescriptorHeap> mRenderTargetDescriptorHeap = nullptr;
			unique_ptr<ShaderResourceDescriptorHeap> mShaderResourceDescriptorHeap = nullptr;
			unique_ptr<ConstBuffer> mConstBuffer = nullptr;
			
			unique_ptr<GpuResource> mVertexBufferResource = nullptr;
			unique_ptr<GpuResource> mIndexBufferResource = nullptr;
			D3D12_VERTEX_BUFFER_VIEW mVertexBufferView;
			D3D12_INDEX_BUFFER_VIEW mIndexBufferView;
			size_t mIndexNum = 0;
		};
	}
}
