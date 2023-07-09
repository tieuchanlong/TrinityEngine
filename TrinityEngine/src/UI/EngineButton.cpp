#include <DirectX11BasePch.h>
#include "UI/EngineButton.h"
#include "EngineLevelManager.h"
#include "EngineAccessor.h"
#include "EngineAccessor.h"

int EngineButton::m_iNumButtons = 0;
EngineButton::EngineButton(XMFLOAT3 vPos, XMFLOAT3 vSize) : EnginePanel(vPos, vSize)
{
	snprintf(m_sObjectName, 7, "Button%i", m_iNumButtons + 1);
	m_iNumButtons++;
}

EngineButton::EngineButton(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor) : EnginePanel(vPos, vSize, vColor)
{
	snprintf(m_sObjectName, 7, "Button%i", m_iNumButtons + 1);
	m_iNumButtons++;
}

void EngineButton::Subscribe(const std::function<void()>& newFunc)
{
	btnDispatcher.push_back(newFunc);
}

void EngineButton::UnSubscribe(const ButtonFuncType& btnFunc)
{

}

int EngineButton::OnMousePressed(const MousePressedEvent& mousePressedEvent)
{
	int iRet = EngineBaseUI::OnMousePressed(mousePressedEvent);

	if (IsInUISpace(mousePressedEvent.GetMousePos()))
	{
		iRet = 1;
		Post();
	}
	else
	{
		iRet = -1;
	}

	return iRet;
}

int EngineButton::OnMouseEnter(XMFLOAT3 vMousePos)
{
	int iRet = EnginePanel::OnMouseEnter(vMousePos);

	return iRet;
}

int EngineButton::OnMouseExit(XMFLOAT3 vMousePos)
{
	int iRet = EnginePanel::OnMouseExit(vMousePos);

	return iRet;
}

int EngineButton::Destroy()
{
	int iRet = EnginePanel::Destroy();

	btnDispatcher.clear();
	return iRet;
}

int EngineButton::Initialize()
{
	int iRet = EnginePanel::Initialize();
	return iRet;
}

int EngineButton::Update(float fDeltaTime)
{
	int iRet = EnginePanel::Update(fDeltaTime);

	return iRet;
}

void EngineButton::Post()
{
	auto&& observers = btnDispatcher.begin();

	// Trigger all other functions linked to this event type
	for (auto&& observer : btnDispatcher)
	{
		observer();
	}
}

// void EngineButton::TestButtonDemo()
// {
// 	Subscribe(std::bind(&Object::OnButtonPressed, GET_LEVEL_MANAGER()->GetCurrentLevel()->GetObjectByIndex(1)));
// }
