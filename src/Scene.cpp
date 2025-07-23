#include <assimp\scene.h>

#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "TextureGroup.h"
#include "PlatformWindow.h"

namespace sr
{
	namespace rendering
	{
		// Precondition
		// - assumes there's only one model as the program's currently using assimp
		// - assumes rendering only one DamagedHelmet, so ignoring recursive node-processing
		// TODO: Switch to scene graph when needed
		Scene::Scene(const aiScene& scene) noexcept
			: mModel()
			, mCamera(nullptr)
			, mLightPosition({0, 6, 1})
		{
			constexpr XMFLOAT3 modelWorldPosition(0, 4, 2);
			mModel = make_unique<Model>(scene, modelWorldPosition);

			constexpr XMFLOAT3 cameraWorldPosition(0, 6, 0);
			constexpr XMFLOAT3 worldUpDirection(0, 1, 0);
			constexpr float fovAngle = 45.0f;
			constexpr float nearZcoordinate = 0.01f;
			constexpr float farZcoordinate = 100.0f;
			mCamera = make_unique<Camera>(fovAngle, platform::aspectRatio, nearZcoordinate, farZcoordinate, cameraWorldPosition);
		}

		void Scene::Update()
		{
			static float angle = 0;
			// TODO: Add camera movement and update camera position
			// TODO: Add proper LightSource class
			//		- mLightPosition is XMMATRIX just to follow const buffer size constraints
			XMMATRIX lightPositions = {};
			const XMVECTOR rotatedVector = XMVector4Transform(XMLoadFloat3(&mLightPosition), XMMatrixRotationZ(angle));
			XMStoreFloat3(&mLightPosition, rotatedVector);

			if (angle >= 360) {
				angle = 0;
				return;
			}
			angle += 0.01f;
		}
	}
}