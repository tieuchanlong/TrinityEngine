#pragma once
#include "EngineSaveData.h"
#include "UI/EngineBaseUI.h"

#define MAX_NUM_UI 100

struct EngineUIData : public SaveData
{
	EngineUIData();

	virtual bool Deserialize(rapidjson::Value::ConstValueIterator& itr) override;
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const override;

	virtual void SetData(EngineBaseUI* pBaseUI);

	int iUIType;
	float x, y, sizeX, sizeY, colorR, colorG, colorB;
	const char* text;
};