#include "Entity.h"

namespace sr
{
	namespace rendering
	{
		class Transform
		{
		public:

		private:
			XMFLOAT3 mPosition = { 0, 0, 0 };
			// TODO: XMFLOAT3 mRotation = { 0, 0, 0, }; 
			// TODO: XMFLOAT3 mScale = { 0 ,0 ,0 };
		};

		Entity::Entity(const aiScene& assimpScene, const XMFLOAT3 worldCoords) noexcept
			: Model(assimpScene, worldCoords)
		{

		}

		void Entity::AddChild()
		{

		}
	}
}