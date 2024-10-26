#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(const float fovAngle, const float aspectRatio, const float nearZCoords, const float farZCoords, const DirectX::XMFLOAT3& worldCoords) noexcept;

	XMMATRIX GetViewLookAtMatrixLh(const XMFLOAT3& eyeCoords, const XMFLOAT3& targetCoords, const XMFLOAT3& upDireciton) const;
	XMMATRIX GetProjectionMatrixLh() const;

private:
	float mFovAngle = 0.0f;
	float mAspectRatio = 0.0f;
	float mNearZCoords = 0.0f;
	float mFarZCoords = 0.0f;

	XMFLOAT3 mWorldCoords = {};
};