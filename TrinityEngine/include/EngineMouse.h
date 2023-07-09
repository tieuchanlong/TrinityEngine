#pragma once
#include "Mouse.h"

using namespace DirectX;

class EngineMouse
{
public:
	static EngineMouse* GetInstance();

	virtual int Initialize();
	virtual int Update(float fDeltaTime);
	virtual int Destroy();

	virtual Mouse::State GetState() { return m_pMouse->GetState(); }
	virtual Mouse::ButtonStateTracker GetTracker() { return m_pTracker; }

	virtual bool IsTriggered() { return m_bTriggered; }

	virtual void SetActive(bool bActive) { m_bActive = bActive; }
	virtual bool IsActive() { return m_bActive; }

protected:
	EngineMouse();

	static EngineMouse* instance;
	std::unique_ptr<Mouse> m_pMouse;
	Mouse::ButtonStateTracker m_pTracker;

	XMFLOAT2 m_vLastMousePos;
	bool m_bTriggered;
	bool m_bActive;
};