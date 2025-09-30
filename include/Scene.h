#pragma once

#include <DirectXMath.h>

#include "Common.h"
struct aiScene;

namespace sr
{
	namespace rendering
	{
		static constexpr DirectX::XMFLOAT3 worldUpDirection(0, 1, 0);

		class AssetLoader;
		class Camera;
		class Entity;
		class Model;

		class Scene
		{
		public:
			Scene(const AssetLoader& assetLoader) noexcept;
			~Scene() = default;

			DirectX::XMVECTOR GetLightPos() const noexcept;
			void Update() noexcept;

			inline Model& GetModel()
			{
				return *mModel;
			}
			inline Camera& GetCamera()
			{
				return *mCamera;
			}
			
		private:
			std::vector<Entity> mEntities = {};
			unique_ptr<Model> mModel = nullptr; // TODO: to be deprecated
			unique_ptr<Camera> mCamera = nullptr;
			const DirectX::XMFLOAT3 mLightPosition;
			float mAngle = 0;
		};
	}
}