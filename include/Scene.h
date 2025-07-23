#pragma once

#include <DirectXMath.h>

#include "Common.h"

struct aiScene;

namespace sr
{
	namespace rendering
	{
		static constexpr DirectX::XMFLOAT3 worldUpDirection(0, 1, 0);

		class Model;
		class Camera;

		class Scene
		{
		public:
			Scene(const aiScene& scene) noexcept;
			~Scene() = default;

			Model& GetModel()
			{
				return *mModel;
			}
			Camera& GetCamera()
			{
				return *mCamera;
			}

			void Update();

		private:
			unique_ptr<Model> mModel = nullptr;
			unique_ptr<Camera> mCamera = nullptr;
			XMFLOAT3 mLightPosition;

			bool mIsDirty;
		};
	}
}