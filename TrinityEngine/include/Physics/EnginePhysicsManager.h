#pragma once

class EnginePhysicsManager
{
public:
	static EnginePhysicsManager* GetInstance();

	XMFLOAT3 GetGravity() { return m_vGravity; }

protected:
	static EnginePhysicsManager* instance;
	EnginePhysicsManager();

	XMFLOAT3 m_vGravity;
};