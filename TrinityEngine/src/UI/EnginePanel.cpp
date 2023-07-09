#include <DirectX11BasePch.h>
#include "UI/EnginePanel.h"
#include "EngineTextComponent.h"
#include "Application.h"

int EnginePanel::m_iNumPanels = 0;
EnginePanel::EnginePanel(XMFLOAT3 vPos, XMFLOAT3 vSize) : EngineBaseUI(vPos, vSize)
{
	snprintf(m_sObjectName, 7, "Panel%i", m_iNumPanels + 1);
	m_iNumPanels++;

	XMVECTOR vCameraChangedVector = Application::GetInstance()->GetMainCamera()->GetFocusPoint() - Application::GetInstance()->GetMainCamera()->GetOriginalFocusPoint();
	XMFLOAT3 vCamerChanged;
	XMStoreFloat3(&vCamerChanged, vCameraChangedVector);
	vPos = vPos + vCamerChanged;

	m_pRectComp = new EngineRectangleComponent(vPos, vSize);
	m_vMainColor = m_vHighlightColor = m_pRectComp->GetColor();
	AddComponent(m_pRectComp);
}

EnginePanel::EnginePanel(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor) : EngineBaseUI(vPos, vSize)
{
	snprintf(m_sObjectName, 7, "Panel%i", m_iNumPanels + 1);
	m_iNumPanels++;

	XMVECTOR vCameraChangedVector = Application::GetInstance()->GetMainCamera()->GetFocusPoint() - Application::GetInstance()->GetMainCamera()->GetOriginalFocusPoint();
	XMFLOAT3 vCamerChanged;
	XMStoreFloat3(&vCamerChanged, vCameraChangedVector);
	vPos = vPos + vCamerChanged;

	m_pRectComp = new EngineRectangleComponent(vPos, vSize, vColor);
	m_vMainColor = m_vHighlightColor = m_pRectComp->GetColor();
	AddComponent(m_pRectComp);
}

bool EnginePanel::IsInUISpace(const XMFLOAT3 vPos)
{
	XMFLOAT3 vRectCompScreenPixelPos = Application::GetInstance()->GetMainCamera()->GetPositionInCameraPixelViewSpace(m_pRectComp->GetLocalPosition());
	float fLocalCameraScreenWidth = Application::GetInstance()->GetMainCamera()->GetCameraLocalViewWidth(m_pRectComp->GetLocalPosition().z);
	float fLocalCameraScreenHeight = Application::GetInstance()->GetMainCamera()->GetCameraLocalViewHeight(m_pRectComp->GetLocalPosition().z);

	RECT clientRect;
	GetClientRect(*Application::GetInstance()->GetWindowHandle(), &clientRect);

	// This is required for a correct projection matrix.
	float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

	float fRectCompPixelWidth = m_pRectComp->GetSize().x * clientWidth / fLocalCameraScreenWidth;
	float fRectCompPixelHeight = m_pRectComp->GetSize().y * clientHeight / fLocalCameraScreenHeight;

	if (vPos.x >= vRectCompScreenPixelPos.x - fRectCompPixelWidth
		&& vPos.x <= vRectCompScreenPixelPos.x + fRectCompPixelWidth
		&& vPos.y >= vRectCompScreenPixelPos.y - fRectCompPixelHeight
		&& vPos.y <= vRectCompScreenPixelPos.y + fRectCompPixelHeight)
	{
		int a = 0;
		a++;
	}

	return (vPos.x >= vRectCompScreenPixelPos.x - fRectCompPixelWidth
		&& vPos.x <= vRectCompScreenPixelPos.x + fRectCompPixelWidth
		&& vPos.y >= vRectCompScreenPixelPos.y - fRectCompPixelHeight
		&& vPos.y <= vRectCompScreenPixelPos.y + fRectCompPixelHeight);
}

int EnginePanel::OnMousePressed(const MousePressedEvent& mousePressedEvent)
{
	int iRet = EngineBaseUI::OnMousePressed(mousePressedEvent);

	if (IsInUISpace(mousePressedEvent.GetMousePos()))
	{
		iRet = 1;
	}
	else
	{
		iRet = -1;
	}

	return iRet;
}

int EnginePanel::OnMouseEnter(XMFLOAT3 vMousePos)
{
	int iRet = EngineBaseUI::OnMouseEnter(vMousePos);

	OnHighlighted();

	return iRet;
}

int EnginePanel::OnMouseExit(XMFLOAT3 vMousePos)
{
	int iRet = EngineBaseUI::OnMouseExit(vMousePos);

	OnUnHighlighted();

	return iRet;
}

int EnginePanel::OnHighlighted()
{
	int iRet = EngineBaseUI::OnHighlighted();
	m_pRectComp->SetColor(m_vHighlightColor);
	return iRet;
}

int EnginePanel::OnUnHighlighted()
{
	int iRet = EngineBaseUI::OnUnHighlighted();
	m_pRectComp->SetColor(m_vMainColor);
	return iRet;
}
