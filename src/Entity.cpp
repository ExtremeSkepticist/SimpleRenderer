#include "Entity.h"
#include "DirectXMath.h"

namespace sr
{
	namespace rendering
	{
		EntityNode::EntityNode(const EntityNode& parent) noexcept
			: mTransforms({})
			, mMeshes({})
			, mParent(&parent)
			, mChildren({})
			, mIsDirty(true)
		{
			const size_t reservedChildrenNum = 128;
			mChildren.reserve(reservedChildrenNum);
			mTransforms.reserve(reservedChildrenNum);
			mMeshes.reserve(reservedChildrenNum);
		}

		EntityNode& EntityNode::AddChild() noexcept
		{
			return mChildren.emplace_back(*this);
		}

		void EntityNode::AddMesh(std::vector<Mesh::Vertex>& vertices, std::vector<IndexType>& indices) noexcept
		{
			mMeshes.emplace_back(vertices, indices);
		}

		Entity::Entity() noexcept
			: mRootNode(std::make_unique<EntityNode>())
		{
		}

		EntityNode& Entity::GetRootNode()
		{
			return *mRootNode;
		}
	}
}