#pragma once

#include "Common.h"

struct aiScene;

namespace sr
{
	namespace rendering
	{
		class Model;
		class Camera;

		class Scene
		{
		public:
			Scene(const aiScene& scene) noexcept;
			~Scene() = default;

			std::vector<Model>& GetModels()
			{
				return mModels;
			}

			void Update();

		private:
			std::vector<Model> mModels = {};
			unique_ptr<Camera> mCamera = nullptr;

			bool mIsDirty;
		};
	}
}