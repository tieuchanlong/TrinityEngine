#pragma once
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
extern class Object;

class Component
{
public:
	Component();

	// --------------- Required for every object --------//
	static Component* Create() { return new Component(); }
	static void RegisterGameClass();
	virtual const char* GetComponentClassName() { return "Component"; }
	virtual int GetComponentClassNameHash() { return ToStringHash("Component"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr);
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	// -------------------------------------------------//

	virtual int Initialize();
	virtual int PreUpdate();
	virtual int Update(float fDeltaTime);
	virtual int Destroy();
	virtual int Render();

	virtual bool IsActive() { return m_bActive; }
	virtual void SetActive(bool bActive);

	virtual void SetOwner(Object* pOwner) { m_pOwner = pOwner; }
	virtual Object* GetOwner() { return m_pOwner; }
protected:
	bool m_bDestroyed; // Check if the component is destroyed
	Object* m_pOwner;

	bool m_bActive;
};