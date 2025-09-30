#pragma once

#include "Common.h"

namespace sr
{
	namespace rendering
	{
		class Transform;
		class Mesh;

		class Node
		{
		public:

		private:
			unique_ptr<Transform> mTransform = nullptr;
			unique_ptr<Mesh> mMesh = nullptr;
			// TODO: unique_ptr<Material> mMaterial = nullptr;
		};

		// TODO: Putting worldCoordinate in Entity, not in Model..And I want to do it via Transform class. How?
		class Entity
		{
		public:
			Entity() noexcept;

			Entity() = delete;
			~Entity() = default;

			void AddChild();
			void Update();
		private:
			void updateRecursively();

			Node* mParent = nullptr;
			std::vector<Node> mChildren = {};

			bool mIsDirty = true;
		};
	}
}
