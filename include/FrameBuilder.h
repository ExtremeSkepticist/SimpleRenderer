#pragma once

#include <d3d12.h>
#include <unordered_map>

#include "Common.h"
#include "Mesh.h"

namespace sr
{
	namespace rendering
	{
		struct CpuConstBufferData;
		
		class Camera;
		class ConstBuffer;
		class GraphicsDevice;
		class GraphicsPipelineState;
		class GpuResource;
		class RenderableNode;
		class RenderTargetDescriptorHeap;
		class RenderingPipeline;
		class RootSignature;
		class Scene;
		class ShaderResourceDescriptorHeap;
		class Mesh;
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

			void Build(RenderableNode& node, Camera& camera, XMFLOAT3 targetPosition, GraphicsDevice& device) noexcept;

			D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView(Mesh& mesh);
			D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView(Mesh& mesh);

			// TODO: abstract this
			ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;
			ComPtr<ID3D12Resource> mDsvBuffer = nullptr;


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
			
			std::unordered_map<const Mesh*, std::pair<GpuResource, D3D12_VERTEX_BUFFER_VIEW>> mVertexResourceMap = {};
			std::unordered_map<const Mesh*, std::pair<GpuResource, D3D12_INDEX_BUFFER_VIEW>> mIndexResourceMap = {};

			size_t mIndexNum = 0;
		};
	}
}
