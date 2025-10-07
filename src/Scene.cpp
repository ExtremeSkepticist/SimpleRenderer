#include <assimp\scene.h>

#include "AssetLoader.h"
#include "Camera.h"
#include "Entity.h"
#include "Model.h"
#include "Scene.h"
#include "TextureGroup.h"
#include "PlatformWindow.h"

namespace sr
{
	namespace rendering
	{
		Scene::Scene(AssetLoader& assetLoader) noexcept
			: mModel(nullptr)
			, mEntities({})
			, mLightPosition({0, 6, 1})
			, mAngle(0)
		{
			const size_t entityCount = assetLoader.GetAssetCount();
			mEntities.reserve(entityCount);
			for (size_t i = 0; i < entityCount; ++i)
			{
				auto& entity = mEntities.emplace_back(std::make_unique<Entity>(true));
				assetLoader.LoadEntity(*entity.get(), i);
			}

			// TODO: support multiple entities in the scene
			constexpr XMFLOAT3 modelWorldPosition(0, 4, 2);
			mEntities[0]->SetPosition(modelWorldPosition);

			constexpr XMFLOAT3 cameraWorldPosition(0, 6, 0);
			constexpr XMFLOAT3 worldUpDirection(0, 1, 0);
			constexpr float fovAngle = 45.0f;
			constexpr float nearZcoordinate = 0.01f;
			constexpr float farZcoordinate = 100.0f;
			std::unique_ptr<Camera> camera = make_unique<Camera>(fovAngle, platform::aspectRatio, nearZcoordinate, farZcoordinate, cameraWorldPosition);
			camera->SetPosition(cameraWorldPosition);
			mEntities.emplace_back(std::move(camera));
		}

		DirectX::XMVECTOR Scene::GetLightPos() const noexcept
		{
			return XMVector4Transform(XMLoadFloat3(&mLightPosition), XMMatrixRotationZ(mAngle));
		}

		void Scene::Update() noexcept
		{
			// TODO: Add camera movement and update camera position
			// TODO: Add proper LightSource class
			//		- mLightPosition is XMMATRIX just to follow const buffer size constraints
			for (auto& entity : mEntities)
			{
				entity->Update();
			}

			if (mAngle >= 360) {
				mAngle = 0;
				return;
			}
			mAngle += 0.01;
		}
	}
}