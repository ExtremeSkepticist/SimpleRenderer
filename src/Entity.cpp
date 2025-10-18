#include "Entity.h"
#include "Material.h"
#include "DirectXMath.h"

namespace sr
{
	namespace rendering
	{
		Transform::Transform() noexcept
			: mPosition({ 0, 0, 0 })
			, mWorldMatrix(XMMatrixIdentity())
		{
		}

		XMFLOAT3 Transform::GetPosition() const noexcept
		{
			return mPosition;
		}

		XMMATRIX Transform::GetWorldMatrix() const noexcept
		{
			return mWorldMatrix;
		}

		void Transform::SetPosition(const XMFLOAT3& position) noexcept
		{
			mPosition = position;
		}

		void Transform::SetScale(const XMFLOAT3& scale) noexcept
		{
			mScale = scale;
		}

		void Transform::Update(const Transform* parent) noexcept
		{
			computeWorldMatrix();

			if (parent)
			{
				mWorldMatrix = XMMatrixMultiply(mWorldMatrix, parent->mWorldMatrix);
			}
		}

		void Transform::computeWorldMatrix() noexcept
		{
			// TODO: Add Rotation and Scale
			
			mWorldMatrix = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

			/*mWorldMatrix = XMMatrixMultiply(XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z)
				, XMMatrixScaling(mScale.x, mScale.y, mScale.z));*/
		}


		// Precondition: every transform, mesh, material uses the first element in general, unless there's a special reason
		// - Only supporting the data as an array just because assimp does so
		EntityNode::EntityNode() noexcept
			: mTransforms({})

		{
			mTransforms.reserve(mReserveSize);

			// Precondition: every node has at least one transform in the vector by default
			mTransforms.emplace_back();
		}

		XMFLOAT3 Entity::GetPosition() const noexcept
		{
			return mRootNode->GetPosition();
		}

		XMFLOAT3 EntityNode::GetPosition() const noexcept
		{
			return mTransforms[0].GetPosition();
		}

		XMMATRIX EntityNode::GetWorldMatrix() const noexcept
		{
			return mTransforms[0].GetWorldMatrix();
		}

		void EntityNode::Update() noexcept
		{
			// TODO: Update transforms for non-renderable entity
			GetTransform().Update(nullptr);
		}

		void EntityNode::SetPosition(const XMFLOAT3& position) noexcept
		{
			mTransforms[0].SetPosition(position);
		}

		void EntityNode::SetScale(const XMFLOAT3& scale) noexcept
		{
			mTransforms[0].SetScale(scale);
		}

		Transform& EntityNode::GetTransform() noexcept
		{
			return mTransforms[0];
		}

		RenderableNode::RenderableNode(RenderableNode* parent) noexcept
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

		void RenderableNode::AddMesh(std::string_view name, std::vector<Mesh::Vertex>& vertices, std::vector<IndexType>& indices, std::shared_ptr<Material> material) noexcept
		{
			mMeshes.emplace_back(name, vertices, indices, material);
		}


		bool RenderableNode::HasChildren() const noexcept
		{
			return !mChildren.empty();
		}

		bool RenderableNode::HasMesh() const noexcept
		{
			return !mMeshes.empty();
		}

		std::vector<RenderableNode>& RenderableNode::GetChildren() noexcept
		{
			return mChildren;
		}

		std::vector<Mesh>& RenderableNode::GetMesh() noexcept
		{
			return mMeshes;
		}

		void RenderableNode::GetNodes(std::vector<RenderableNode*>& nodes) noexcept
		{
			nodes.emplace_back(this);
			for (auto& child : mChildren)
			{
				child.GetNodes(nodes);
			}
		}

		void RenderableNode::Update() noexcept
		{
			// TODO: Update child node if Entity needs an update
			if (mParent)
			{
				GetTransform().Update(&mParent->GetTransform());
			}
			else
			{
				GetTransform().Update(nullptr);
			}

			for (auto& child : mChildren)
			{
				child.Update();
			}
		}

		Entity::Entity(const bool isRenderable) noexcept
			: mIsRenderable(isRenderable)
			, mRootNode(mIsRenderable
				? std::make_unique<RenderableNode>(nullptr) : std::make_unique<EntityNode>())
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

		void Entity::SetScale(const XMFLOAT3& scale) noexcept
		{
			mRootNode->SetScale(scale);
		}

		void Entity::GetRenderableNodes(std::vector<RenderableNode*>& nodes) noexcept
		{
			if (mIsRenderable)
			{
				reinterpret_cast<RenderableNode*>(mRootNode.get())->GetNodes(nodes);
			}
		}

		EntityNode& Entity::GetRootNode() noexcept
		{
			return *mRootNode;
		}

		void Entity::Update() noexcept
		{
			// TODO: Remove manual update
			// TODO: check keyboard/mouse input and do update
			// TODO: Consider add dirty flag to nodes as well to avoid unnecessary update
			if (mIsDirty)
			{
				mRootNode->Update();
				mIsDirty = false;
			}
		}
	}
}