#include <DirectX11BasePch.h>
#include "Physics/EngineRigidBodyComponent.h"
#include "Physics/EnginePhysicsManager.h"
#include "EngineLevelManager.h"
#include "EngineComponentManager.h"
#include "EngineAccessor.h"

EngineRigidComponent::EngineRigidComponent() : Component()
{
	m_fMass = 0.001f;
	m_vSpeed = XMFLOAT3(0, 0, 0);
}

void EngineRigidComponent::RegisterGameClass()
{
	EngineComponentManager::GetInstance()->Register(ToStringHash("EngineRigidComponent"), EngineRigidComponent::Create);
}

bool EngineRigidComponent::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	bool bRet = Component::ReadSaveData(itr);

	const rapidjson::Value& obj = (*itr);

	m_vSpeed = XMFLOAT3(obj["SpeedX"].GetDouble(), obj["SpeedY"].GetDouble(), obj["SpeedZ"].GetDouble());
	m_fMass = obj["Mass"].GetDouble();
	itr++;

	return bRet;
}

bool EngineRigidComponent::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	bool bRet = Component::WriteSaveData(writer);

	writer->StartObject();
	writer->Key("SpeedX");
	writer->Double(m_vSpeed.x);
	writer->Key("SpeedY");
	writer->Double(m_vSpeed.y);
	writer->Key("SpeedZ");
	writer->Double(m_vSpeed.z);
	writer->Key("Mass");
	writer->Double(m_fMass);
	writer->EndObject();

	return true;
}

DirectX::XMFLOAT3 EngineRigidComponent::GetSpeed()
{
	if (m_pCollidingObj)
	{
		return XMFLOAT3(0, 0, 0);
	}

	return m_vSpeed + m_fMass * EnginePhysicsManager::GetInstance()->GetGravity();
}

int EngineRigidComponent::Update(float fDeltaTime)
{
	int iRet = Component::Update(fDeltaTime);

	if (m_vSpeed.x != 0 || m_vSpeed.y != 0 || m_vSpeed.z != 0 || m_fMass > 0)
	{
		// Check for collision
		m_pCollidingObj = GET_LEVEL_MANAGER()->GetCurrentLevel()->GetCollidingObject(GetOwner());
	}

	return 0;
}

