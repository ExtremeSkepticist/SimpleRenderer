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
			, mCamera(nullptr)
			, mLightPosition({0, 6, 1})
			, mAngle(0)
		{
			const size_t entityCount = assetLoader.GetAssetCount();
			mEntities.reserve(entityCount);
			for (size_t i = 0; i < entityCount; ++i)
			{
				Entity& entity = mEntities.emplace_back();
				assetLoader.LoadEntity(entity, i);
			}

			// TODO: move worldPosition into entity
			// TODO: Should camera belong to entity? 
			//	- Base class (has transform, parent, children, isDirty)
			//		-> inherited by EntityNode (has meshes)
			//		-> inherited by Camera class (has camera functions)
			constexpr XMFLOAT3 modelWorldPosition(0, 4, 2);
			// mModel = make_unique<Model>(scene, modelWorldPosition);

			constexpr XMFLOAT3 cameraWorldPosition(0, 6, 0);
			constexpr XMFLOAT3 worldUpDirection(0, 1, 0);
			constexpr float fovAngle = 45.0f;
			constexpr float nearZcoordinate = 0.01f;
			constexpr float farZcoordinate = 100.0f;
			mCamera = make_unique<Camera>(fovAngle, platform::aspectRatio, nearZcoordinate, farZcoordinate, cameraWorldPosition);
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
			if (mAngle >= 360) {
				mAngle = 0;
				return;
			}
			mAngle += 0.01;
		}
	}
}