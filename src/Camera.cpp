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

		// TODO: fix this with https://chatgpt.com/c/6976ebbf-0cf4-8330-b6fd-7caac18979e1
		XMMATRIX Camera::GetViewLookToMatrixLh(const XMFLOAT3& upDirection) const noexcept
		{
			const XMFLOAT3 cameraPos = GetPosition();
			const XMFLOAT3 targetPos = mTargetPosition;
			const XMVECTOR cameraVec = XMLoadFloat3(&cameraPos);
			const XMVECTOR targetVec = XMLoadFloat3(&targetPos);

			const XMVECTOR lookToDirection = XMVector3Normalize(targetVec - cameraVec);

			return XMMatrixLookToLH(cameraVec, lookToDirection, XMLoadFloat3(&upDirection));
		}

		XMMATRIX Camera::getRotationMatrix(const int xDelta, const int yDelta, XMVECTOR upDirection)
		{
			const XMFLOAT3 cameraPos = GetPosition();
			const XMFLOAT3 targetPos = mTargetPosition;
			const XMVECTOR cameraVec = XMLoadFloat3(&cameraPos);
			const XMVECTOR targetVec = XMLoadFloat3(&targetPos);

			const XMVECTOR forwardVec = XMVector3Normalize(targetVec - cameraVec);
			const XMVECTOR rightVec = XMVector3Cross(forwardVec, upDirection);

			return XMMatrixRotationRollPitchYaw(0, xDelta, yDelta);
		}
		XMMATRIX Camera::GetProjectionMatrixLh() const noexcept
		{
			return XMMatrixPerspectiveFovLH(mFovAngle, mAspectRatio, mNearZCoords, mFarZCoords);
		}

		void Camera::SetOrientationDelta(const float xDelta, const float yDelta) noexcept
		{
			mDeltaX = xDelta;
			mDeltaY = yDelta;
		}

		void Camera::SetTargetPosition(const XMFLOAT3& targetPosition) noexcept
		{
			mTargetPosition = targetPosition;
		}

		void Camera::Update() noexcept
		{
			mTargetPosition = XMStoreFloat3(XMVector3Transform(XMLoadFloat3(&mTargetPosition), getRotationMatrix()));
			//TODO: re-compute mTargetPosition with the current orientation
			// TODO: Update camera world-space position if delta exists by rotating them
		}
	}
}