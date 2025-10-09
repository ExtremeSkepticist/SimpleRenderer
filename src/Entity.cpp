#include "Entity.h"
#include "Material.h"
#include "DirectXMath.h"

namespace sr
{
	namespace rendering
	{
		Transform::Transform() noexcept
			: mPosition({0, 0, 0})
			, mWorldMatrix(XMMatrixIdentity())
		{
		}

		void Transform::SetPosition(const XMFLOAT3& position) noexcept
		{
			mPosition = position;
		}

		XMMATRIX Transform::GetWorldMatrix() const noexcept
		{
			return mWorldMatrix;
		}

		void Transform::Update(const Transform* parent) noexcept
		{
			computeWorldMatrix();

			if (parent)
			{
				XMMatrixMultiply(mWorldMatrix, parent->mWorldMatrix);
			}
		}

		void Transform::computeWorldMatrix() noexcept
		{
			// TODO: Add Rotation and Scale
			mWorldMatrix = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
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

		void EntityNode::Update() noexcept
		{
			// TODO: Update transforms for non-renderable entity
			GetTransform().Update(nullptr);
		}

		void EntityNode::SetPosition(const XMFLOAT3& position) noexcept
		{
			mTransforms[0].SetPosition(position);
		}

		Transform& EntityNode::GetTransform() noexcept
		{
			return mTransforms[0];
		}

		RenderableNode::RenderableNode(RenderableNode* parent)
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
			mMeshes.emplace_back(vertices, indices, material);
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
			// TODO: Consider add dirty flag to nodes as well to avoid unnecessary update
			if (mIsDirty)
			{
				mRootNode->Update();
				mIsDirty = false;
			}
		}
	}
}