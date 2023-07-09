#include <DirectX11BasePch.h>
#include "Event/EngineEvent.h"

void Dispatcher::Subcribe(const EventType& eventType, const FuncType& func)
{
	m_Observers[eventType].push_back(func);
}

void Dispatcher::Post(const Event& event) const
{
	auto type = event.GetType();

	// Ignore events for which we do not have an observer (yet).
	if (m_Observers.find(type) == m_Observers.end())
	{
		return;
	}

	auto&& observers = m_Observers.at(type);

	// Trigger all other functions linked to this event type
	for (auto&& observer : observers)
	{
		observer(event);
	}
}

void Dispatcher::Unsubcribe(const EventType& eventType, const FuncType& func)
{
	// Implement ID so we can remove the function with ID comparison
}
