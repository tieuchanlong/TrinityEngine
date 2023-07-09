#pragma once
#include "EngineObject.h"
#include "SaveData/EngineLevelData.h"

#define MAX_LOADING_OBJECTS_PER_FRAME 100


struct ObjectTemporaryData
{
	XMFLOAT3 vPos;
	const char* sObjectSource;
};

class Level : public Object
{
public:
	Level();

	// --------------- Required for every object --------//
	static Object* Create() { return new Level(); }
	static void RegisterGameClass();
	static const char* GetObjectClassName() { return "Level"; }
	static int GetObjectClassNameHash() { return ToStringHash("Level"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr);
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	// -------------------------------------------------//

	// Collision management
	Object* GetObjectByIndex(int iIndex, bool bActive = true);
	Object* GetCollidingObject(Object* pObject);
	int GetNumObjects() { return m_pObjects->GetNumElements(); }

	virtual int Initialize() override;
	virtual int PreUpdate() override;
	virtual int Update(float fDeltaTime) override;
	virtual int Render() override;
	virtual int Destroy() override;

	virtual void LoadObjects();
	virtual bool IsDoneLoadingObjects() { return !m_bLoadingObjects || m_iCurLoadingObjIndex >= m_pObjectTempData->GetNumElements(); }
	virtual void AddObject(Object* pObject);

	void SetLevelIndex(int iLevelIndex) { m_iLevelIndex = iLevelIndex; }

	virtual void SetHidden(bool bHidden) override;

	virtual void OnResumeGame();
	virtual void OnCancelPressed();

protected:
	// Objects management
	List<Object>* m_pObjects;
	List<ObjectTemporaryData>* m_pObjectTempData;
	bool m_bLoadingObjects;
	int m_iCurLoadingObjIndex;
	int m_iLevelIndex;
};