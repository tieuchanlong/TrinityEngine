#include <DirectX11BasePch.h>
#include "EngineKeyboard.h"

EngineKeyboard* EngineKeyboard::instance = nullptr;
EngineKeyboard::EngineKeyboard()
{
	m_bTriggered = false;
}

EngineKeyboard* EngineKeyboard::GetInstance()
{
	if (!instance)
	{
		instance = new EngineKeyboard();
	}

	return instance;
}

int EngineKeyboard::Initialize()
{
	int iRet = 0;

	// Create keyboard and mouse
	m_pKeyboard = std::make_unique<Keyboard>();

	return iRet;
}

int EngineKeyboard::Update(float fDeltaTime)
{
	int iRet = 0;

	m_pTracker.Update(m_pKeyboard->GetState());

	if (m_pTracker.pressed.W || m_pTracker.pressed.A || m_pTracker.pressed.S || m_pTracker.pressed.D || m_pTracker.pressed.Up || m_pTracker.pressed.Down || m_pTracker.pressed.Left || m_pTracker.pressed.Right)
	{
		m_bTriggered = true;
	}
	else
	{
		m_bTriggered = false;
	}

	return iRet;
}

int EngineKeyboard::Destroy()
{
	return 0;
}
