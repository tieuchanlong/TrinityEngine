#pragma once
#include "Event/EngineEvent.h"

class SelectPrevOptionEvent : public Event
{
public:
	virtual EventType GetType() const { return EventType::SelectPrevOption; }
};

class SelectNextOptionEvent : public Event
{
public:
	virtual EventType GetType() const { return EventType::SelectNextOption; }
};

class ConfirmOptionEvent : public Event
{
public:
	virtual EventType GetType() const { return EventType::ConfirmOption; }
};

class SelectRightOptionEvent : public Event
{
public:
	virtual EventType GetType() const { return EventType::SelectRightOption; }
};

class SelectLeftOptionEvent : public Event
{
public:
	virtual EventType GetType() const { return EventType::SelectLeftOption; }
};

class ChangeMenuEvent : public Event
{
public:
	virtual EventType GetType() const { return EventType::ChangeMenu; }
};

class CancelPressedEvent : public Event
{
public:
	virtual EventType GetType() const { return EventType::CancelPressed; }
};