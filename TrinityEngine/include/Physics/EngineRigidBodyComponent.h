#pragma once
#include "EngineComponent.h"


class EngineRigidComponent : public Component
{
public:
	EngineRigidComponent();

	// --------------- Required for every component --------//
	static Component* Create() { return new EngineRigidComponent(); }
	static void RegisterGameClass();
	virtual const char* GetComponentClassName() override { return "EngineRigidComponent"; }
	virtual int GetComponentClassNameHash() override { return ToStringHash("EngineRigidComponent"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr) override;
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer) override;
	// -------------------------------------------------//

	XMFLOAT3 GetSpeed();

	virtual int Update(float fDeltaTime) override;

protected:
	XMFLOAT3 m_vSpeed;
	float m_fMass;
	Object* m_pCollidingObj;
};