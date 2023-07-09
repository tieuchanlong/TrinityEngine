#include <DirectX11BasePch.h>
#include "Event/EngineEventManager.h"
#include "EngineMouse.h"
#include "Event/MousePressedEvent.h"
#include "Event/MenuSelectOptionEvent.h"

#include "EngineKeyboard.h"

EngineEventManager* EngineEventManager::instance = nullptr;
EngineEventManager* EngineEventManager::GetInstance()
{
	if (!instance)
	{
		instance = new EngineEventManager();
	}

	return instance;
}

int EngineEventManager::Initialize()
{
	return 0;
}

int EngineEventManager::Update(int fDeltaTime)
{
	auto mouseState = EngineMouse::GetInstance()->GetState();
	auto mouseTracker = EngineMouse::GetInstance()->GetTracker();
	auto kbTracker = EngineKeyboard::GetInstance()->GetTracker();

	if (EngineMouse::GetInstance()->IsTriggered())
	{
		m_pCurInputType = InputType::MOUSE_INPUT;
	}

	if (EngineKeyboard::GetInstance()->IsTriggered())
	{
		m_pCurInputType = InputType::KEYBOARD_INPUT;
	}

	if (mouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
	{
		m_bMouseClicked = false;
	}

	if (!m_bMouseClicked && mouseState.leftButton && m_Dispatchers[EventType::MouseButtonPressed] && EngineMouse::GetInstance()->IsActive())
	{
		m_bMouseClicked = true;
		MousePressedEvent mousePressedEvent;
		mousePressedEvent.SetMousePos(XMFLOAT3(mouseState.x, mouseState.y, 0));

		m_Dispatchers[EventType::MouseButtonPressed]->Post(mousePressedEvent);
	}

	if (kbTracker.pressed.Up)
	{
		SelectPrevOptionEvent selectPrevOptionEvent;

		m_Dispatchers[EventType::SelectPrevOption]->Post(selectPrevOptionEvent);
	}

	if (kbTracker.pressed.Down)
	{
		SelectNextOptionEvent selectNextOptionEvent;

		m_Dispatchers[EventType::SelectNextOption]->Post(selectNextOptionEvent);
	}

	if (kbTracker.pressed.Left)
	{
		SelectLeftOptionEvent selectLeftOptionEvent;

		m_Dispatchers[EventType::SelectLeftOption]->Post(selectLeftOptionEvent);
	}

	if (kbTracker.pressed.Right)
	{
		SelectRightOptionEvent selectRightOptionEvent;

		m_Dispatchers[EventType::SelectRightOption]->Post(selectRightOptionEvent);
	}

	if (kbTracker.released.Enter)
	{
		m_bConfirmPressed = false;
	}

	if (!m_bConfirmPressed && kbTracker.pressed.Enter)
	{
		m_bConfirmPressed = true;
		ConfirmOptionEvent confirmOptionEvent;

		m_Dispatchers[EventType::ConfirmOption]->Post(confirmOptionEvent);
	}

	if (kbTracker.pressed.Tab)
	{
		ChangeMenuEvent changeMenuEvent;

		m_Dispatchers[EventType::ChangeMenu]->Post(changeMenuEvent);
	}

	if (kbTracker.released.Escape)
	{
		m_bCanceledPressed = false;
	}

	if (!m_bCanceledPressed && kbTracker.pressed.Escape)
	{
		m_bCanceledPressed = true;
		CancelPressedEvent cancelPressedEvent;

		m_Dispatchers[EventType::CancelPressed]->Post(cancelPressedEvent);
	}

	return 0;
}

int EngineEventManager::Destroy()
{
	return 0;
}

void EngineEventManager::SubscribeFunction(EventType eventType, const Dispatcher::FuncType& func)
{
	if (m_Dispatchers[eventType] == NULL)
	{
		m_Dispatchers[eventType] = new Dispatcher();
	}

	m_Dispatchers[eventType]->Subcribe(eventType, func);
}

void EngineEventManager::UnSubscriveFunction(EventType eventType, Dispatcher::FuncType& func)
{

}

EngineEventManager::EngineEventManager()
{
	m_pCurInputType = InputType::MOUSE_INPUT;
	m_bCanceledPressed = false;
	m_bConfirmPressed = false;
	m_bMouseClicked = false;
}

