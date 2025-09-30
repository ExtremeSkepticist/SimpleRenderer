#include "Entity.h"
#include "Mesh.h"
#include "DirectXMath.h"

namespace sr
{
	namespace rendering
	{
		class Transform
		{
		public:

		private:
			DirectX::XMFLOAT3 mPosition = { 0, 0, 0 };
			// TODO: XMFLOAT3 mRotation = { 0, 0, 0, }; 
			// TODO: XMFLOAT3 mScale = { 0 ,0 ,0 };
		};

		Entity::Entity() noexcept
		{

		}

		void Entity::AddChild()
		{

		}
	}
}