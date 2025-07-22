#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class ConstBuffer;
		class GraphicsDevice;
		class GraphicsPipelineState;
		class RenderTargetDescriptorHeap;
		class RenderingPipeline;
		class RootSignature;
		class Scene;
		class ShaderResourceDescriptorHeap;
		class Model;
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

			inline Model* getModel() const noexcept
			{
				return mModel;
			}

			inline void setModel(gsl::not_null<Model*> model) noexcept
			{
				mModel = model;
			}

			// TODO: abstract this
			ComPtr<ID3D12DescriptorHeap> mDsvHeap = nullptr;
			ComPtr<ID3D12Resource> mDsvBuffer = nullptr;

		private:
			void createDefaultResources(RenderingPipeline& pipeline, Scene& scene);
			void createDefaultRootSignature(const GraphicsDevice& device);
			void createDefaultPipelineState(const GraphicsDevice& device);
			void recordCommands();

			unique_ptr<RootSignature> mRootSignature = nullptr;
			unique_ptr<GraphicsPipelineState> mPipelineState = nullptr;
			unique_ptr<RenderTargetDescriptorHeap> mRenderTargetDescriptorHeap = nullptr;
			unique_ptr<ShaderResourceDescriptorHeap> mShaderResourceDescriptorHeap = nullptr;
			unique_ptr<GpuResource> mVertexBufferResource = nullptr;
			unique_ptr<GpuResource> mIndexBufferResource = nullptr;
			unique_ptr<ConstBuffer> mConstBuffer = nullptr;
			Model* mModel = nullptr;
		};
	}
}
