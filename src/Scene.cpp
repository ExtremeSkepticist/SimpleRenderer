#include <assimp\scene.h>

#include "AssetLoader.h"
#include "Camera.h"
#include "Entity.h"
#include "FrameBuilder.h"
#include "RenderingPipeline.h"
#include "Scene.h"
#include "TextureGroup.h"
#include "PlatformWindow.h"
#include "UserInput.h"

namespace sr
{
	namespace rendering
	{
		Scene::Scene(AssetLoader& assetLoader) noexcept
			: mEntities({})
			, mLightPosition({0, 6, 1})
			, mAngle(0)
			, mCamera(nullptr)
		{
			const size_t entityCount = assetLoader.GetAssetCount();
			mEntities.reserve(entityCount);
			for (size_t i = 0; i < entityCount; ++i)
			{
				auto& entity = mEntities.emplace_back(std::make_shared<Entity>(true));
				assetLoader.LoadEntity(*entity.get(), i);
			}

			// TODO: support multiple entities in the scene
			constexpr XMFLOAT3 modelWorldPosition(0, 4, 2);
			constexpr XMFLOAT3 modelScale(1, 1, 1);

			mEntities[0]->SetPosition(modelWorldPosition);
			mEntities[0]->SetScale(modelScale);

			constexpr XMFLOAT3 cameraWorldPosition(-3.5, 5, 0);
			constexpr XMFLOAT3 worldUpDirection(0, 1, 0);
			constexpr float fovAngle = 45.0f;
			constexpr float nearZcoordinate = 0.01f;
			constexpr float farZcoordinate = 100.0f;
			std::shared_ptr<Camera> camera = make_shared<Camera>(fovAngle, platform::aspectRatio, nearZcoordinate, farZcoordinate);
			camera->SetPosition(cameraWorldPosition);
			camera->SetTargetPosition(modelWorldPosition);
			mEntities.emplace_back(std::move(camera));

			mCamera = reinterpret_cast<Camera*>(mEntities[1].get());
		}

		DirectX::XMVECTOR Scene::GetLightPos() const noexcept
		{
			return XMVector4Transform(XMLoadFloat3(&mLightPosition), XMMatrixRotationZ(mAngle));
		}

		void Scene::Update(UserInput& userInput) noexcept
		{
			// TODO: Add camera movement and update camera position
			// TODO: Add proper LightSource class
			//		- mLightPosition is XMMATRIX just to follow const buffer size constraints
			auto cameraPosition = mCamera->GetPosition();
			if (userInput.mPressedAscend)
			{
				cameraPosition.y += 0.1;
			}
			else if (userInput.mPressedDescend)
			{
				cameraPosition.y -= 0.1;
			}
			else if (userInput.mPressedForward)
			{
				cameraPosition.z += 0.1;
			}
			else if (userInput.mPressedBackward)
			{
				cameraPosition.z -= 0.1;
			}
			else if (userInput.mPressedLeft)
			{
				cameraPosition.x -= 0.1;
			}
			else if (userInput.mPressedRight)
			{
				cameraPosition.x += 0.1;
			}
			else if (userInput.mCursorDeltaX > 0 || userInput.mCursorDeltaY)
			{
				// TODO: set delta for Camera, and update it in Update() (i.e. update camera position to be rotated)
			}
			mCamera->SetPosition(cameraPosition);

			for (auto& entity : mEntities)
			{
				entity->Update();
			}
		}

		void Scene::Render(FrameBuilder& frameBuilder, RenderingPipeline& pipeline) noexcept
		{
			pipeline.RenderStart(frameBuilder);

			for (auto& entity : mEntities)
			{
				if (!entity->IsRenderable())
				{
					[[unlikely]]
					continue;
				}
				
				// TODO: Is there a way to separate rendering logic from Scene?
				const XMFLOAT3 position = entity->GetPosition();
				std::vector<RenderableNode*> nodes;
				nodes.reserve(128); // TODO: replace the magic number
				entity->GetRenderableNodes(nodes);

				for (auto& node: nodes)
				{					
					for (auto& mesh : node->GetMesh())
					{
						frameBuilder.Build(mesh, *node, *mCamera, position, pipeline.GetGraphicsDevice());
						pipeline.Render(frameBuilder, mesh);
					}
				}
			}

			pipeline.RenderEnd();
		}
	}
}