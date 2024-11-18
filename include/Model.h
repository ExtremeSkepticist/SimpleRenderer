#pragma once

#include <d3d12.h>

#include "Common.h"
#include "Mesh.h"

struct aiScene;

namespace sr
{
	namespace rendering
	{
		class GraphicsDevice;

		class Model
		{
		public:
			Model() = delete;
			Model(const aiScene& assimpScene, const XMFLOAT3 worldCoords, const GraphicsDevice& graphicsDevice) noexcept;
			~Model() = default;

			inline vector<unique_ptr<Mesh>>& GetMeshes() noexcept
			{
				return mMeshes;
			}

			inline XMFLOAT3 GetWorldCoords() const noexcept
			{
				return mWorldCoords;
			}

		private:
			vector<unique_ptr<Mesh>> mMeshes = {};
			XMFLOAT3 mWorldCoords = {};
		};
	}
}

