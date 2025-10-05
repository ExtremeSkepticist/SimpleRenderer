#pragma once

#include "Common.h"
#include "Mesh.h"

namespace sr
{
	namespace rendering
	{
		class Mesh;
		class Material;

		class Transform
		{
		public:

		private:
			DirectX::XMFLOAT3 mPosition = { 0, 0, 0 };
			// TODO: XMFLOAT3 mRotation = { 0, 0, 0, }; 
			// TODO: XMFLOAT3 mScale = { 0 ,0 ,0 };
		};

		class EntityNode
		{
		public:
			EntityNode() noexcept;
			EntityNode() = delete;
			virtual ~EntityNode() = default;

			virtual void Update();

		protected:
			static constexpr uint32_t mReserveSize = 128;

		private:
			std::vector<Transform> mTransforms = {};
		};

		class RenderableNode final: public EntityNode
		{
		public:
			RenderableNode(const RenderableNode* parent) noexcept;
			RenderableNode() = delete;
			~RenderableNode() override = default;

			RenderableNode& AddChild() noexcept;
			void AddMesh(std::vector<Mesh::Vertex>& vertices, std::vector<IndexType>& indices, std::unique_ptr<Material>& material) noexcept;
			void Update() noexcept override;

		private:
			// TODO
			void updateRecursively() noexcept;

			const RenderableNode* mParent = nullptr;
			std::vector<RenderableNode> mChildren = {};
			std::vector<Mesh> mMeshes = {};
		};

		// Entity = renderable
		// Camera = Entity with Camera functions, non-renderable
		class Entity
		{
		public:
			Entity(const bool isRenderable) noexcept;
			Entity() = delete;
			~Entity() = default;

			EntityNode& GetRootNode();
			bool IsRenderable() const noexcept;

			// TODO: Check keyboard/mouse input and do update
			virtual void Update() noexcept;

		private:
			std::unique_ptr<EntityNode> mRootNode = nullptr;
			const bool mIsRenderable = false;
			bool mIsDirty = true;
		};
	}
}
