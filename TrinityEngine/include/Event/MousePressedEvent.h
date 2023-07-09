#pragma once
#include "Event/EngineEvent.h"

class MousePressedEvent : public Event
{
public:
	MousePressedEvent();

	virtual EventType GetType() const { return EventType::MouseButtonPressed; }
	void SetMousePos(XMFLOAT3 vMousePos) { m_vMousePos = vMousePos; }
	 XMFLOAT3 GetMousePos() const { return m_vMousePos; }

protected:
	XMFLOAT3 m_vMousePos;
};