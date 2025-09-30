#include "Entity.h"
#include "Mesh.h"
#include "DirectXMath.h"

namespace sr
{
	namespace rendering
	{


		EntityNode::EntityNode(const EntityNode& parent) noexcept
			: mTransform(std::make_unique<Transform>())
			, mMesh(std::make_unique<Mesh>())
			, mParent(&parent)
			, mChildren({})
			, mIsDirty(true)
		{
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