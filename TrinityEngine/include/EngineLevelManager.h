#pragma once
#include "EngineLevel.h"
#include "Event/EngineEventManager.h"

class LevelData : public SaveData
{
	friend class EngineLevelManager;

public:
	virtual bool Deserialize(rapidjson::Value::ConstValueIterator& itr) override;
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const override;

protected:
	Level* m_pLevel;
};


class EngineLevelManager
{
	friend class LevelData;

public:
	EngineLevelManager();

	virtual int Initialize();
	virtual int PreUpdate();
	virtual int Update(float fDeltaTime);
	virtual int Render();
	virtual int Destroy();

	virtual Level* CreateLevel(const char* sLevelPath);
	Level* GetCurrentLevel() { return m_pCurLevel; }

	virtual void AddLevel(Level* pLevel);

	virtual void SaveLevel(Level* pLevel);
	virtual Level* GetLevelByIndex(int iLevelIndex) { return m_pLevels->GetElementAtIndex(iLevelIndex)->GetValue(); }

	virtual bool IsDoneLoadingLevels();

	virtual void OnCancelPressed(const Event& e);

protected:

	// UIs management
	List<Level>* m_pLevels;
	Level* m_pCurLevel;

	LevelData m_pLevelData;
	bool m_bTransitionMenuActive;
};