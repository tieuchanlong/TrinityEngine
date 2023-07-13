#include <DirectX11BasePch.h>
#include "EngineMouse.h"
#include "Application.h"

EngineMouse* EngineMouse::instance = nullptr;
EngineMouse* EngineMouse::GetInstance()
{
	if (!instance)
	{
		instance = new EngineMouse();
	}

	return instance;
}

int EngineMouse::Initialize()
{
	m_pMouse = new Mouse();
	m_pMouse->SetWindow(*Application::GetInstance()->GetWindowHandle());
	m_vLastMousePos = XMFLOAT2(m_pMouse->GetState().x, m_pMouse->GetState().y);

	return 0;
}

int EngineMouse::Update(float fDeltaTime)
{
	auto mouseState = m_pMouse->GetState();

	if (!IsActive())
	{
		m_bTriggered = false;
	}

	m_pTracker.Update(m_pMouse->GetState());

	if (m_vLastMousePos.x != mouseState.x || m_vLastMousePos.y != mouseState.y || mouseState.leftButton || mouseState.rightButton || mouseState.middleButton)
	{
		m_vLastMousePos = XMFLOAT2(mouseState.x, mouseState.y);
		m_bTriggered = true;
	}
	else
	{
		m_bTriggered = false;
	}

	return 0;
}

int EngineMouse::Destroy()
{
	return 0;
}

EngineMouse::EngineMouse()
{
	m_bTriggered = false;
	m_bActive = true;
}
