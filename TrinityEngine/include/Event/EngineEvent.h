#pragma once
#include <functional>
#include <map>
#include <vector>

#include "EngineList.h"

enum class EventType
{
	None = 0,
	MouseButtonPressed,
	MouseButtonReleased,
	SelectPrevOption,
	SelectNextOption,
	SelectRightOption,
	SelectLeftOption,
	ConfirmOption,
	ChangeMenu,
	CancelPressed,
	NumEventTypes
};

class Event
{
public:
	virtual EventType GetType() const { return EventType::None; }
};

class Dispatcher
{
public:
	using FuncType = std::function<void(const Event&)>;
	void Subcribe(const EventType& eventType, const FuncType& func);
	void Post(const Event& event) const;
	void Unsubcribe(const EventType& eventType, const FuncType& func);

protected:
	std::map< EventType, std::vector<FuncType> > m_Observers;
};