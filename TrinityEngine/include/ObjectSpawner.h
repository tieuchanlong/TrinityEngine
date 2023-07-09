#pragma once
#include "EngineObject.h"
#include "EngineLevel.h"

#define MAX_SPAWN_OBJECTS 50

class ObjectSpawner : public Object
{
public:
	ObjectSpawner();

	// --------------- Required for every object --------//
	static ObjectSpawner* Create() { return new ObjectSpawner(); }
	static void RegisterGameClass();
	virtual const char* GetObjectClassName() { return "ObjectSpawner"; }
	virtual int GetObjectClassNameHash() { return ToStringHash("ObjectSpawner"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr);
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	// -------------------------------------------------//

	virtual void SpawnObjects();
	virtual void SpawnHighlights();
	virtual bool IsDoneSpawning() {
		return (m_bCanSpawnObjects && m_iCurSpawnObjIndex >= Quantity && m_iCurHighlightX > MaxX && m_iCurHighlightX > MaxX) || (!m_bCanSpawnObjects && m_iCurHighlightX > MaxX);
	}

	virtual int PreUpdate() override;
	virtual int Destroy() override;

	virtual void AddObjectSource(const char* sObjectSource) { m_sSpawnObjectSources[m_iSpawnObjectSourceCount] = sObjectSource; m_iSpawnObjectSourceCount++; }
	virtual void SetSpawnDimension(int iMinX, int iMaxX, int iMinY, int iMaxY, int iMinZ, int iMaxZ) {
		MinX = iMinX; MaxX = iMaxX; MinY = iMinY; MaxY = iMaxY; MinZ = iMinZ; MaxZ = iMaxZ; m_iCurHighlightX = MinX;
		m_iCurHighlightY = MinY;
		m_iCurHighlightZ = MinZ;
	}
	virtual void SetQuantity(int iQuantity) { Quantity = iQuantity; }

	virtual void SetCanSpawnObjects(bool bCanSpawnObjects) { m_bCanSpawnObjects = bCanSpawnObjects; }

	virtual void ShowSpawnRegionHighlights(bool bShowHighlight);

	void SetLevelOwner(Level* pLevelOwner) { m_pLevelOwner = pLevelOwner; }

protected:
	int MinX, MaxX, MinY, MaxY, MinZ, MaxZ;
	int Quantity;
	const char* m_sSpawnObjectSources[MAX_SPAWN_OBJECTS];
	int m_iSpawnObjectSourceCount;
	int m_iCurSpawnObjIndex;
	bool m_bCanSpawnObjects;
	bool m_bShowHighlights;

	int m_iCurHighlightX;
	int m_iCurHighlightY;
	int m_iCurHighlightZ;

	List<Object>* m_pSpawnObjs;
	List<Object>* m_pSpawnRegionHighlights;

	Level* m_pLevelOwner;
};