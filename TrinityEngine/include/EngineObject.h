#pragma once
#include "EngineList.h"
#include "EngineRenderComponent.h"
#include "SaveData/EngineSaveData.h"
#include "Physics/EngineBoundingBoxComponent.h"

class Object
{
public:
	Object();

	// --------------- Required for every object --------//
	static Object* Create() { return new Object(); }
	static void RegisterGameClass();
	virtual const char* GetObjectClassName() { return "Object"; }
	virtual int GetObjectClassNameHash() { return ToStringHash("Object"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr);
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	// -------------------------------------------------//

	virtual bool IsInitialized() { return m_bIsInitialized; }
	virtual int Initialize();
	virtual int PreUpdate();
	virtual int Update(float fDeltaTime);
	virtual int Destroy();
	virtual int Render();

	virtual bool IsActive() { return m_bActive; }
	virtual void SetActive(bool bActive);

	virtual bool IsHidden() { return m_bHidden; }
	virtual void SetHidden(bool bHidden);

	virtual void AddComponent(Component* component);
	
	const char* GetObjectName() { return m_sObjectName; }
	void SetObjectName(char* sObjectName) { m_sObjectName = sObjectName; }

	virtual void SetWorldPosition(XMFLOAT3 vPos) { m_vPos = vPos; }
	virtual XMFLOAT3 GetWorldPosition();
	virtual XMFLOAT3 GetSize();
	virtual XMFLOAT3 GetColor();

	virtual void SetObjectSource(const char* sObjectSource) { m_sObjectSource = sObjectSource; }
	virtual const char* GetObjectSource() { return m_sObjectSource; }

	virtual bool HasRenderComponent();
	virtual bool HasRigidBodyComponent();

	template <class compType>
	compType* GetComponentByType()
	{
		// Initialize components
		for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
		{
			if (dynamic_cast<compType*>(it->GetValue()))
			{
				return dynamic_cast<compType*>(it->GetValue());
			}
		}

		return NULL;
	}

	// UI Interactions
	virtual void OnButtonPressed();

	virtual AABB GetBoundingDimension();

protected:
	EngineRenderComponent* m_pRenderComponent;
	List<Component>* m_pComponents;

	// Update details
	XMFLOAT3 m_vPos;

	bool m_bActive;
	bool m_bDestroyed; // Check if the object is destroyed

	bool m_bHidden;

	char* m_sObjectName;
	const char* m_sObjectSource;
	AABB m_AABB;

	bool m_bIsInitialized;
private:
	void TestComponents();
	static int m_iNumObjects;
};