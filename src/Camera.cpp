#include "Camera.h"

Camera::Camera(float fovAngle, float aspectRatio, float nearZCoords, float farZCoords, const XMFLOAT3& worldCoords) noexcept
	: mFovAngle(fovAngle)
	, mAspectRatio(aspectRatio)
	, mNearZCoords(nearZCoords)
	, mFarZCoords(farZCoords)
	, mWorldCoords(worldCoords)
{
}

XMMATRIX Camera::GetViewLookAtMatrixLh(const XMFLOAT3& eyeCoords, const XMFLOAT3& targetCoords, const XMFLOAT3& upDirection) const
{
	return XMMatrixLookAtLH(XMLoadFloat3(&eyeCoords), XMLoadFloat3(&targetCoords), XMLoadFloat3(&upDirection));;
}

XMMATRIX Camera::GetProjectionMatrixLh() const
{
	return XMMatrixPerspectiveFovLH(mFovAngle, mAspectRatio, mNearZCoords, mFarZCoords);
}
