#pragma once
#include <Windows.h>

#include "Common.h"
#include "Mesh.h"

namespace sr
{
	namespace rendering
	{
		class Material;

		class Transform
		{
		public:
			Transform() noexcept;
			
			XMFLOAT3 GetPosition() const noexcept;
			XMMATRIX GetWorldMatrix() const noexcept;

			void SetPosition(const XMFLOAT3& position) noexcept;
			
			void Update(const Transform* parent) noexcept;

		private:
			void computeWorldMatrix() noexcept;

			XMFLOAT3 mPosition = { 0, 0, 0 };
			XMMATRIX mWorldMatrix = XMMatrixIdentity();

			// TODO: XMFLOAT3 mRotation = { 0, 0, 0, }; 
			// TODO: XMFLOAT3 mScale = { 0 ,0 ,0 };
		};

		class EntityNode
		{
		public:
			EntityNode() noexcept;
			virtual ~EntityNode() = default;

			XMFLOAT3 GetPosition() const noexcept;
			XMMATRIX GetWorldMatrix() const noexcept;
			
			void SetPosition(const XMFLOAT3& position) noexcept;

			virtual void Update() noexcept;

		protected:
			static constexpr uint32_t mReserveSize = 128;
			Transform& GetTransform() noexcept;

		private:
			std::vector<Transform> mTransforms = {};
		};

		class RenderableNode final: public EntityNode
		{
		public:
			RenderableNode(RenderableNode* parent) noexcept;
			RenderableNode() = delete;
			~RenderableNode() override = default;

			RenderableNode& AddChild() noexcept;
			void AddMesh(std::string_view name, std::vector<Mesh::Vertex>& vertices, std::vector<IndexType>& indices, std::unique_ptr<Material> material) noexcept;
			
			bool HasChildren() const noexcept;
			Mesh& GetMesh() noexcept;
			std::vector<RenderableNode>& GetChildren() noexcept;
			void GetNodes(std::vector<RenderableNode*>& nodes) noexcept;

			void Update() noexcept override;

		private:
			RenderableNode* mParent = nullptr;
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

			bool IsRenderable() const noexcept;
			void GetRenderableNodes(std::vector<RenderableNode*>& nodes) noexcept;
			XMFLOAT3 GetPosition() const noexcept;
			EntityNode& GetRootNode() noexcept;

			void SetPosition(const XMFLOAT3& position) noexcept;

			// TODO: Check keyboard/mouse input and do update
			virtual void Update() noexcept;

		private:
			std::unique_ptr<EntityNode> mRootNode = nullptr;
			const bool mIsRenderable = false;
			bool mIsDirty = true;
		};
	}
}
