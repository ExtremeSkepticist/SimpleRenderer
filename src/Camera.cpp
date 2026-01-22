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

		XMMATRIX Camera::GetViewLookToMatrixLh(const XMFLOAT3& eyeCoords, const XMFLOAT3& upDirection) const noexcept
		{
			return XMMatrixLookToLH(XMLoadFloat3(&eyeCoords), XMLoadFloat3(&mTargetPosition), XMLoadFloat3(&upDirection));
		}

		XMMATRIX Camera::GetProjectionMatrixLh() const noexcept
		{
			return XMMatrixPerspectiveFovLH(mFovAngle, mAspectRatio, mNearZCoords, mFarZCoords);
		}

		void Camera::SetTargetPosition(const XMFLOAT3& targetPosition) noexcept
		{
			mTargetPosition = targetPosition;

			XMStoreFloat3(&mTargetPosition, XMVector3Normalize(XMLoadFloat3(&mTargetPosition)));
		}

		void Camera::Update() noexcept
		{
			//TODO: add update code by entity update request
		}
	}
}