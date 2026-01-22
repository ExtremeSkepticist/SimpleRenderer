#pragma once

#include <Windows.h>
#include <DirectXMath.h>

#include "Entity.h"

namespace sr
{
	namespace rendering
	{
		using namespace DirectX;

		class Camera : public Entity
		{
		public:
			Camera(const float fovAngle, const float aspectRatio, const float nearZCoords, const float farZCoords) noexcept;
			~Camera();

			XMMATRIX GetViewLookAtMatrixLh(const XMFLOAT3& eyeCoords, const XMFLOAT3& targetCoords, const XMFLOAT3& upDireciton) const noexcept;
			XMMATRIX GetViewLookToMatrixLh(const XMFLOAT3& eyeCoords, const XMFLOAT3& upDirection) const noexcept;
			XMMATRIX GetProjectionMatrixLh() const noexcept;

			void SetTargetPosition(const XMFLOAT3& targetPosition) noexcept;
			void Update() noexcept override;
		private:
			float mFovAngle = 0.0f;
			float mAspectRatio = 0.0f;
			float mNearZCoords = 0.0f;
			float mFarZCoords = 0.0f;

			XMFLOAT3 mTargetPosition = {};
		};
	}
}
