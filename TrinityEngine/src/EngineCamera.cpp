#include <DirectX11BasePch.h>
#include "EngineCamera.h"
#include "EngineKeyboard.h"
#include "EngineMouse.h"
#include "Application.h"

#define WRONG_POS_BUFFER_X 70
#define WRONG_POS_BUFFER_Y 5

XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

Camera::Camera()
{
	m_vEyePosition = XMVectorSet(0, 0, -20, 1); // Making the camera moving 10 units back looking towards the origin
	m_vOriginalFocusPoint = m_vFocusPoint = XMVectorSet(0, 0, 0, 1);

	m_vForwardDirection = XMVectorSet(0, 0, 1, 0);
	m_vRightDirection = XMVectorSet(1, 0, 0, 0);
	m_vUpDirection = XMVectorSet(0, 1, 0, 0);

	m_fCameraSpeed = .005f;
	m_fCameraRotateSpeed = 0.005f;
	m_fCurCameraYawAngle = 0;
	m_fCurCameraPitchAngle = 0;
	m_vMousePos.x = m_vMousePos.y = -1;

	m_pCamRotationMatrix = XMMatrixRotationRollPitchYaw(0, 0, 0);
	m_bLockedTranslation = false;
	m_bLockedZoom = false;
	m_bLockedRotation = false;

	m_bIsCameraUpdated = false;
	m_ViewMatrix = XMMatrixLookAtLH(m_vEyePosition, m_vFocusPoint, m_vUpDirection);

	// Setup the projection matrix.
	// This is required for a correct projection matrix.
	float clientWidth = Application::GetInstance()->GetViewPort()->Width;
	float clientHeight = Application::GetInstance()->GetViewPort()->Height;
	m_pProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(GetFOVAngle()), clientWidth / clientHeight, GetNearZDist(), GetFarZDist());
}

float Camera::GetCameraLocalViewWidth(float zDist)
{
	XMFLOAT3 vEyePosition;
	XMStoreFloat3(&vEyePosition, m_vEyePosition);
	float fNearZScreenWidth = (-vEyePosition.z + zDist) * tan(XMConvertToRadians(GetAdjustedFOVAngle())); // Long: May 15th, have to fix this temporary 5 unit buffer so the mouse match the box dimension

	return fNearZScreenWidth * 2;
}

float Camera::GetCameraLocalViewHeight(float zDist)
{
	float fNearZScreenWidth = GetCameraLocalViewWidth(zDist);

	float clientWidth = Application::GetInstance()->GetViewPort()->Width;
	float clientHeight = Application::GetInstance()->GetViewPort()->Height;
	float fNearZScreenHeight = fNearZScreenWidth * clientHeight / clientWidth;

	return fNearZScreenHeight;
}

DirectX::XMFLOAT3 Camera::GetPositionInCameraLocalViewSpace(XMFLOAT3 vWorldPosition)
{
	XMVECTOR vFWorldPosition = XMLoadFloat3(&vWorldPosition);
	XMVECTOR vFScreenPosition = XMVector3Transform(vFWorldPosition, m_ViewMatrix);
	XMFLOAT3 vScreenPosition;
	XMStoreFloat3(&vScreenPosition, vFScreenPosition);

	return vScreenPosition;
}

DirectX::XMFLOAT3 Camera::GetPositionInCameraPixelViewSpace(XMFLOAT3 vWorldPosition)
{
	vWorldPosition.y = -vWorldPosition.y;
	XMFLOAT3 vScreenPosition = GetPositionInCameraLocalViewSpace(vWorldPosition);
	XMFLOAT3 vEyePosition;
	XMStoreFloat3(&vEyePosition, m_vEyePosition);
	vScreenPosition.z += vEyePosition.z;
	
	float fLocalCameraViewWidth = GetCameraLocalViewWidth(vScreenPosition.z);
	float fLocalCameraViewHeight = GetCameraLocalViewHeight(vScreenPosition.z);

	float clientWidth = Application::GetInstance()->GetViewPort()->Width;
	float clientHeight = Application::GetInstance()->GetViewPort()->Height;

	XMFLOAT3 vScreenPixelPosition = vScreenPosition;
	vScreenPixelPosition.x = (vScreenPosition.x + fLocalCameraViewWidth / 2) * clientWidth / fLocalCameraViewWidth /*+ WRONG_POS_BUFFER_X*/;
	vScreenPixelPosition.y = (vScreenPosition.y + fLocalCameraViewHeight / 2) * clientHeight / fLocalCameraViewHeight /*- WRONG_POS_BUFFER_Y*/;

	return vScreenPixelPosition;
}

int Camera::Update(float fDeltaTime)
{
	int iRet = 0;

	auto kb = EngineKeyboard::GetInstance()->GetState();

	m_bIsCameraUpdated = false;

	if (m_vMousePos.x < 0 || m_vMousePos.y < 0)
	{
		// In this case, the mouse position is not set yet
		m_vMousePos.x = EngineMouse::GetInstance()->GetState().x;
		m_vMousePos.y = EngineMouse::GetInstance()->GetState().y;
	}

	if (EngineMouse::GetInstance()->GetTracker().rightButton == Mouse::ButtonStateTracker::HELD)
	{
		m_vFocusPoint -= m_vEyePosition;
		float yawAngleChanged = 0/*EngineMouse::GetInstance()->GetState().x - m_vMousePos.x*/;
		float pitchAngleChanged = 0/*EngineMouse::GetInstance()->GetState().y - m_vMousePos.y*/;

		// Camera pitch and yaw rotation
		if (kb.Right && !m_bLockedRotation)
		{
			yawAngleChanged += m_fCameraRotateSpeed;
			m_fCurCameraYawAngle += m_fCameraRotateSpeed;
		}

		if (kb.Left && !m_bLockedRotation)
		{
			yawAngleChanged -= m_fCameraRotateSpeed;
			m_fCurCameraYawAngle -= m_fCameraRotateSpeed;
		}

		if (kb.Up && !m_bLockedRotation)
		{
			pitchAngleChanged += m_fCameraRotateSpeed;
			m_fCurCameraPitchAngle += m_fCameraRotateSpeed;
		}

		if (kb.Down && !m_bLockedRotation)
		{
			pitchAngleChanged -= m_fCameraRotateSpeed;
			m_fCurCameraPitchAngle -= m_fCameraRotateSpeed;
		}

		if (yawAngleChanged != 0 || pitchAngleChanged != 0)
		{
			m_pCamRotationMatrix = XMMatrixRotationRollPitchYaw(m_fCurCameraPitchAngle, m_fCurCameraYawAngle, 0);
			m_vFocusPoint = XMVector3TransformCoord(DefaultForward, m_pCamRotationMatrix);

			XMMATRIX RotateTempMatrix;
			RotateTempMatrix = XMMatrixRotationY(yawAngleChanged) * XMMatrixRotationX(pitchAngleChanged);

			m_vRightDirection = XMVector3TransformCoord(DefaultRight, RotateTempMatrix);
			m_vUpDirection = XMVector3TransformCoord(m_vUpDirection, RotateTempMatrix);
			m_vForwardDirection = XMVector3TransformCoord(DefaultForward, RotateTempMatrix);

			m_bIsCameraUpdated = true;
		}

		// Camera translation
		if (kb.W && !m_bLockedZoom)
		{
			m_vEyePosition += m_vForwardDirection * m_fCameraSpeed * fDeltaTime;
			m_bIsCameraUpdated = true;
		}

		if (kb.S && !m_bLockedZoom)
		{
			m_vEyePosition -= m_vForwardDirection * m_fCameraSpeed * fDeltaTime;
			m_bIsCameraUpdated = true;
		}

		if (kb.A && !m_bLockedTranslation)
		{
			m_vEyePosition -= m_vRightDirection * m_fCameraSpeed * fDeltaTime;
			m_bIsCameraUpdated = true;
		}

		if (kb.D && !m_bLockedTranslation)
		{
			m_vEyePosition += m_vRightDirection * m_fCameraSpeed * fDeltaTime;
			m_bIsCameraUpdated = true;
		}

		if (kb.Q && !m_bLockedTranslation)
		{
			m_vEyePosition += m_vUpDirection * m_fCameraSpeed * fDeltaTime;
			m_bIsCameraUpdated = true;
		}

		if (kb.E && !m_bLockedTranslation)
		{
			m_vEyePosition -= m_vUpDirection * m_fCameraSpeed * fDeltaTime;
			m_bIsCameraUpdated = true;
		}

		m_vFocusPoint += m_vEyePosition;
		if (m_bIsCameraUpdated)
		{
			m_ViewMatrix = XMMatrixLookAtLH(m_vEyePosition, m_vFocusPoint, m_vUpDirection);

			// Setup the projection matrix.
			// This is required for a correct projection matrix.
			m_ViewMatrix = XMMatrixLookAtLH(m_vEyePosition, m_vFocusPoint, m_vUpDirection);

			Application::GetInstance()->ReRender();
		}
	}
	else if (EngineMouse::GetInstance()->GetTracker().rightButton == Mouse::ButtonStateTracker::ButtonState::RELEASED)
	{
		m_bHoldRightMouse = false;
	}

	m_vMousePos.x = EngineMouse::GetInstance()->GetState().x;
	m_vMousePos.y = EngineMouse::GetInstance()->GetState().y;

	return iRet;
}
