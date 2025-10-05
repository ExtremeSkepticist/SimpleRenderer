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
		class Entity;
		class Model;

		class Scene
		{
		public:
			Scene(AssetLoader& assetLoader) noexcept;
			~Scene() = default;

			DirectX::XMVECTOR GetLightPos() const noexcept;
			void Update() noexcept;

			inline Model& GetModel()
			{
				return *mModel;
			}

			
		private:
			std::vector<std::unique_ptr<Entity>> mEntities = {};
			unique_ptr<Model> mModel = nullptr; // TODO: to be deprecated
			const DirectX::XMFLOAT3 mLightPosition;
			float mAngle = 0;
		};
	}
}