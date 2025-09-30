#pragma once

#include "Common.h"
#include "Model.h"

namespace sr
{
	namespace rendering
	{
		class Transform;
		class Node
		{
		public:

		private:
			unique_ptr<Transform> mTransform = nullptr;
		};

		// TODO: Putting worldCoordinate in Entity, not in Model..And I want to do it via Transform class. How?
		class Entity final: public Model
		{
		public:
			Entity() = delete;
			Entity(const aiScene& assimpScene, const XMFLOAT3 worldCoords) noexcept;

			~Entity() override = default;

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
