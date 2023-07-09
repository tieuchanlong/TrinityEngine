#pragma once
#include <map>
#include "EngineEvent.h"

enum class InputType
{
	MOUSE_INPUT,
	KEYBOARD_INPUT,
	NUM_INPUT_TYPES
};

class EngineEventManager
{
public:
	static EngineEventManager* GetInstance();

	virtual int Initialize();
	virtual int Update(int fDeltaTime);
	virtual int Destroy();

	virtual void SubscribeFunction(EventType eventType, const Dispatcher::FuncType& func);
	virtual void UnSubscriveFunction(EventType eventType, Dispatcher::FuncType& func);

	virtual InputType GetCurInputType() { return m_pCurInputType; }
protected:
	static EngineEventManager* instance;
	EngineEventManager();

	std::map<EventType, Dispatcher*> m_Dispatchers;

	InputType m_pCurInputType;
	bool m_bCanceledPressed;
	bool m_bConfirmPressed;
	bool m_bMouseClicked;
};