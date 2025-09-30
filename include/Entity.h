#pragma once

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class Mesh;
		
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
			EntityNode(const EntityNode& node) noexcept;
			
			EntityNode() = delete;
			~EntityNode() = default;
		private:
			unique_ptr<Transform> mTransform = nullptr;
			unique_ptr<Mesh> mMesh = nullptr;
			// TODO: unique_ptr<Material> mMaterial = nullptr;

			const EntityNode* mParent = nullptr;
			std::vector<EntityNode> mChildren = {};

			bool mIsDirty = true;
		};

		// TODO: Putting worldCoordinate in Entity, not in Model..And I want to do it via Transform class. How?
		class Entity
		{
		public:
			Entity() noexcept;

			Entity() = delete;
			~Entity() = default;

			EntityNode& GetRootNode();
			void Update();
		private:
			unique_ptr<EntityNode> mRootNode = nullptr;

			void updateRecursively();
		};
	}
}
