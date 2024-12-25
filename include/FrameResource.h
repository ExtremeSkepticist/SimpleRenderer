#pragma once

#include <d3d12.h>

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;
		class GraphicsPipelineState;
		class RenderTargetDescriptorHeap;
		class RootSignature;
		class ShaderResourceDescriptorHeap;
		class Model;

		struct DescriptorHandle;

		class FrameResource
		{
		public:
			FrameResource(const GraphicsDevice& device);
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
			void createDefaultRootSignature(const GraphicsDevice& device);
			void createDefaultPipelineState(const GraphicsDevice& device);

			unique_ptr<RenderTargetDescriptorHeap> mRenderTargetDescriptorHeap = nullptr;
			unique_ptr<RootSignature> mRootSignature = nullptr;
			unique_ptr<GraphicsPipelineState> mPipelineState = nullptr;
			unique_ptr<ShaderResourceDescriptorHeap> mShaderResourceDescriptorHeap = nullptr;

			Model* mModel = nullptr;
		};
	}
}
