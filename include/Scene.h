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

			inline Model& GetModel()
			{
				return *mModel;
			}
			inline Camera& GetCamera()
			{
				return *mCamera;
			}
			DirectX::XMVECTOR GetLightPos() const;

			void Update();

		private:
			unique_ptr<Model> mModel = nullptr;
			unique_ptr<Camera> mCamera = nullptr;
			const DirectX::XMFLOAT3 mLightPosition;
			float mAngle = 0;

			bool mIsDirty;
		};
	}
}