#pragma once

using namespace DirectX;

enum CameraRotateDirection
{
	ROTATE_YAW,
	ROTATE_PITCH,
	NUM_ROTATION_DIR
};

class Camera
{
public:
	Camera();

	XMVECTOR GetEyePosition() { return m_vEyePosition; }
	XMVECTOR GetFocusPoint() { return m_vFocusPoint; }
	XMVECTOR GetOriginalFocusPoint() { return m_vOriginalFocusPoint; }
	XMVECTOR GetUpDirection() { return m_vUpDirection; }
	float GetFOVAngle() { return 45.f; }
	float GetAdjustedFOVAngle() { return 36.316f; }
	float GetNearZDist() { return 0.1f; }
	float GetFarZDist() { return 100.f; }
	XMMATRIX GetProjectionMatrix() { return m_pProjectionMatrix; }

	float GetCameraLocalViewWidth(float zDist);
	float GetCameraLocalViewHeight(float zDist);
	XMFLOAT3 GetPositionInCameraLocalViewSpace(XMFLOAT3 vWorldPosition);
	XMFLOAT3 GetPositionInCameraPixelViewSpace(XMFLOAT3 vWorldPosition);

	float GetCameraSpeed() { return m_fCameraSpeed; }
	void SetCameraSpeed(float fNewCameraSpeed) { m_fCameraSpeed = fNewCameraSpeed; }
	float GetCameraRotateSpeed() { return m_fCameraRotateSpeed; }
	void SetCameraRotateSpeed(float fNewCameraRotateSpeed) { m_fCameraRotateSpeed = fNewCameraRotateSpeed; }

	void UpdateCameraPosition(float x, float y, float z) { m_vEyePosition = XMVectorSet(x, y, z, 0); }
	void UpdateCameraFocusPoint(float x, float y, float z) { m_vFocusPoint = XMVectorSet(x, y, z, 0); }

	virtual int Update(float fDeltaTime);

	bool IsCameraUpdated() { return m_bIsCameraUpdated; }

	void SetLockedTranslation(bool bLocked) { m_bLockedTranslation = bLocked; }
	void SetLockedZoom(bool bLocked) { m_bLockedZoom = bLocked; }
	void SetLockedRotation(bool bLocked) { m_bLockedRotation = bLocked; }
	void SetLockedAll(bool bLockedAll) { m_bLockedTranslation = bLockedAll; m_bLockedZoom = bLockedAll; m_bLockedTranslation = bLockedAll; }
	bool IsLockedTranslation() { return m_bLockedTranslation; }
	bool IsLockedZoom() { return m_bLockedZoom; }
	bool IsLockedRotation() { return m_bLockedRotation; }

private:
	XMVECTOR m_vEyePosition;
	XMVECTOR m_vFocusPoint;
	XMVECTOR m_vOriginalFocusPoint;

	XMVECTOR m_vForwardDirection;
	XMVECTOR m_vRightDirection;
	XMVECTOR m_vUpDirection;

	bool m_bIsCameraUpdated;

	float m_fCameraSpeed;
	float m_fCameraRotateSpeed;

	float m_fCurCameraYawAngle;
	float m_fCurCameraPitchAngle;

	bool m_bLockedTranslation;
	bool m_bLockedZoom;
	bool m_bLockedRotation;

	// Mouse
	XMFLOAT2 m_vMousePos;
	bool m_bHoldRightMouse;

	XMMATRIX m_ViewMatrix;
	XMMATRIX m_pProjectionMatrix;
	XMMATRIX m_pCamRotationMatrix;
};