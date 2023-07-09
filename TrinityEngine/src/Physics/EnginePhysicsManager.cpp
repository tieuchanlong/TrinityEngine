#include <DirectX11BasePch.h>
#include "Physics/EnginePhysicsManager.h"

EnginePhysicsManager* EnginePhysicsManager::instance = nullptr;
EnginePhysicsManager* EnginePhysicsManager::GetInstance()
{
	if (!instance)
	{
		instance = new EnginePhysicsManager();
	}

	return instance;
}

EnginePhysicsManager::EnginePhysicsManager()
{
	m_vGravity = XMFLOAT3(0, -9.81, 0);
}
