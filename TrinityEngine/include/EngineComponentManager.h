#pragma once

#include <functional>
#include <map>
#include "EngineComponent.h"

class EngineComponentManager
{
public:
	using CreateCompFuncType = std::function<Component* ()>;

	static EngineComponentManager* GetInstance();
	virtual void Register(int iClassNameHash, CreateCompFuncType classCreateFunc);
	virtual Component* CreateComponent(int iClassNameHash);
protected:
	static EngineComponentManager* instance;
	EngineComponentManager();

	std::map< int, CreateCompFuncType > m_Observers;
};