#pragma once
#include "Keyboard.h"

using namespace DirectX;

class EngineKeyboard
{
public:
	static EngineKeyboard* GetInstance();

	virtual int Initialize();
	virtual int Update(float fDeltaTime);
	virtual int Destroy();

	virtual Keyboard::State GetState() { return m_pKeyboard->GetState(); }
	virtual Keyboard::KeyboardStateTracker GetTracker() { return m_pTracker; }

	virtual bool IsTriggered() { return m_bTriggered; }

protected:
	EngineKeyboard();

	static EngineKeyboard* instance;
	std::unique_ptr<Keyboard> m_pKeyboard;
	Keyboard::KeyboardStateTracker m_pTracker;
	bool m_bTriggered;
};