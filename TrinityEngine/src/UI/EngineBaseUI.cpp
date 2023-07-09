#include <DirectX11BasePch.h>
#include "UI/EngineBaseUI.h"
#include "UI/EngineGUIManager.h"

EngineBaseUI::EngineBaseUI(XMFLOAT3 vPos, XMFLOAT3 vSize) : Object()
{
	m_bMouseEntered = false;
	m_bMouseExit = false;
}

void EngineBaseUI::OnEnter()
{

}

void EngineBaseUI::OnClick()
{

}

void EngineBaseUI::OnExit()
{

}

int EngineBaseUI::OnMouseEnter(XMFLOAT3 vMousePos)
{
	m_bMouseEntered = true;
	m_bMouseExit = false;
	return 0;
}

int EngineBaseUI::OnMouseExit(XMFLOAT3 vMousePos)
{
	m_bMouseEntered = false;
	m_bMouseExit = true;
	return 0;
}
