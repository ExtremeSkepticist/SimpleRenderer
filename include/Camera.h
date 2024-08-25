#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(const float fovAngle, const float aspectRatio, const float nearZCoords, const float farZCoords, const DirectX::XMFLOAT3& worldCoords);

	XMMATRIX GetViewLookAtMatrixLh(const XMFLOAT3& eyeCoords, const XMFLOAT3& targetCoords, const XMFLOAT3& upDireciton) const;
	XMMATRIX GetProjectionMatrixLh() const;

private:
	float mFovAngle;
	float mAspectRatio;
	float mNearZCoords;
	float mFarZCoords;

	XMFLOAT3 mWorldCoords;
};