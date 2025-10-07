#include "Entity.h"
#include "DirectXMath.h"

namespace sr
{
	namespace rendering
	{
		Transform::Transform() noexcept
			: mPosition({0, 0, 0})
		{
		}

		void Transform::SetPosition(const XMFLOAT3& position) noexcept
		{
			mPosition = position;
		}

		// Precondition: every transform, mesh, material uses the first element in general, unless there's a special reason
		EntityNode::EntityNode() noexcept
			: mTransforms({})

		{
			mTransforms.reserve(mReserveSize);
			mTransforms.emplace_back(); // Precondition: every node has at least one transform in the vector
		}

		void EntityNode::Update() noexcept
		{
			// TODO: Update transforms for non-renderable entity
		}

		void EntityNode::SetPosition(const XMFLOAT3& position) noexcept
		{
			mTransforms[0].SetPosition(position);
		}


		RenderableNode::RenderableNode(const RenderableNode* parent)
			: EntityNode()
			, mParent(parent)
			, mChildren({})
			, mMeshes({})
		{
			mChildren.reserve(mReserveSize);
			mMeshes.reserve(mReserveSize);
		}

		RenderableNode& RenderableNode::AddChild() noexcept
		{
			return mChildren.emplace_back(this);
		}

		void RenderableNode::AddMesh(std::vector<Mesh::Vertex>& vertices, std::vector<IndexType>& indices, std::unique_ptr<Material>& material) noexcept
		{
			mMeshes.emplace_back(vertices, indices);
		}

		void RenderableNode::Update() noexcept
		{
			// TODO: Update child node if Entity needs an update
		}

		Entity::Entity(const bool isRenderableNode) noexcept
			: mIsRenderable(isRenderableNode)
			, mRootNode(mIsRenderable
				? std::make_unique<RenderableNode>(nullptr) : std::make_unique<EntityNode>(nullptr))
			, mIsDirty(true)
		{
		}

		bool Entity::IsRenderable() const noexcept
		{
			return mIsRenderable;
		}

		void Entity::SetPosition(const XMFLOAT3& position) noexcept
		{
			mRootNode->SetPosition(position);
		}

		void Entity::Update() noexcept
		{
			// TODO: Remove manual update
			// TODO: check keyboard/mouse input and do update
			if (mIsDirty)
			{
				mRootNode->Update();
				mIsDirty = false;
			}
		}
	}
}