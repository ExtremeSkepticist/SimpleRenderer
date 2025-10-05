#include "Entity.h"
#include "DirectXMath.h"

namespace sr
{
	namespace rendering
	{
		EntityNode::EntityNode() noexcept
			: mTransforms({})
			, mIsDirty(true)

		{
			mTransforms.reserve(mReserveSize);
		}

		void EntityNode::Update() noexcept
		{
			// TODO: Update transforms for non-renderable entity
		}

		RenderableNode::RenderableNode(const RenderableNode* parent)
			: mParent(parent)
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

		EntityNode& Entity::GetRootNode()
		{
			return *mRootNode;
		}

		bool Entity::IsRenderable() const noexcept
		{
			return mIsRenderable;
		}

		void Entity::Update() noexcept
		{
			// TODO: Check keyboard/mouse input and do update
		}
	}
}