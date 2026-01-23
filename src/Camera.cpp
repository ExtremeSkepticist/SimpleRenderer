#include "Camera.h"

namespace sr
{
	namespace rendering
	{
		Camera::Camera(float fovAngle, float aspectRatio, float nearZCoords, float farZCoords) noexcept
			: Entity(false)
			, mFovAngle(fovAngle)
			, mAspectRatio(aspectRatio)
			, mNearZCoords(nearZCoords)
			, mFarZCoords(farZCoords)
			, mTargetPosition({ 0, 0, 0 })
		{
		}

		Camera::~Camera() = default;

		XMMATRIX Camera::GetViewLookAtMatrixLh(const XMFLOAT3& eyeCoords, const XMFLOAT3& targetCoords, const XMFLOAT3& upDirection) const noexcept
		{
			return XMMatrixLookAtLH(XMLoadFloat3(&eyeCoords), XMLoadFloat3(&targetCoords), XMLoadFloat3(&upDirection));
		}

		XMMATRIX Camera::GetViewLookToMatrixLh(const XMFLOAT3& upDirection) const noexcept
		{
			const XMFLOAT3 cameraPos = GetPosition();
			const XMFLOAT3 targetPos = mTargetPosition;
			const XMVECTOR cameraVec = XMLoadFloat3(&cameraPos);
			const XMVECTOR targetVec = XMLoadFloat3(&targetPos);

			const XMVECTOR lookToDirection = XMVector3Normalize(targetVec - cameraVec);

			return XMMatrixLookToLH(cameraVec, lookToDirection, XMLoadFloat3(&upDirection));
		}

		XMMATRIX Camera::GetProjectionMatrixLh() const noexcept
		{
			return XMMatrixPerspectiveFovLH(mFovAngle, mAspectRatio, mNearZCoords, mFarZCoords);
		}

		void Camera::SetTargetPosition(const XMFLOAT3& targetPosition) noexcept
		{
			mTargetPosition = targetPosition;
		}

		void Camera::Update() noexcept
		{
			//TODO: add update code by entity update request
		}
	}
}