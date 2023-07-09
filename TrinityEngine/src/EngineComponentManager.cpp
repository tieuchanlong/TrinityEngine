#include <DirectX11BasePch.h>
#include "EngineComponentManager.h"

EngineComponentManager* EngineComponentManager::instance = 0;
EngineComponentManager* EngineComponentManager::GetInstance()
{
	if (!instance)
	{
		instance = new EngineComponentManager();
	}

	return instance;
}

void EngineComponentManager::Register(int iClassNameHash, CreateCompFuncType classCreateFunc)
{
	Assert(m_Observers.find(iClassNameHash) == m_Observers.end());
	m_Observers[iClassNameHash] = classCreateFunc;
}

Component* EngineComponentManager::CreateComponent(int iClassNameHash)
{
	Assert(m_Observers.find(iClassNameHash) != m_Observers.end());
	return m_Observers[iClassNameHash]();
}

EngineComponentManager::EngineComponentManager()
{

}
