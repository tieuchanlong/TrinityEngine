#include <DirectX11BasePch.h>
#include "SaveData/EngineUIData.h"
#include "UI/EngineText.h"

EngineUIData::EngineUIData()
{
	text = new char();
}

bool EngineUIData::Deserialize(rapidjson::Value::ConstValueIterator& itr)
{
// 	x = obj["x"].GetDouble();
// 	y = obj["y"].GetDouble();
// 	sizeX = obj["sizeX"].GetDouble();
// 	sizeY = obj["sizeY"].GetDouble();
// 	colorR = obj["colorR"].GetDouble();
// 	colorG = obj["colorG"].GetDouble();
// 	colorB = obj["colorB"].GetDouble();
// 	iUIType = obj["uiType"].GetInt();
// 	text = _strdup(obj["uiText"].GetString());

	return true;
}

bool EngineUIData::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
	writer->StartObject();

	writer->Key("x");
	writer->Double(x);

	writer->Key("y");
	writer->Double(y);

	writer->Key("sizeX");
	writer->Double(sizeX);

	writer->Key("sizeY");
	writer->Double(sizeY);

	writer->Key("colorR");
	writer->Double(colorR);

	writer->Key("colorG");
	writer->Double(colorG);

	writer->Key("colorB");
	writer->Double(colorB);

	writer->Key("uiType");
	writer->Int(iUIType);

	writer->Key("uiText");
	writer->String(text);

	writer->EndObject();

	return true;
}

void EngineUIData::SetData(EngineBaseUI* pBaseUI)
{
	x = pBaseUI->GetWorldPosition().x;
	y = pBaseUI->GetWorldPosition().y;

	sizeX = pBaseUI->GetSize().x;
	sizeY = pBaseUI->GetSize().y;

	colorR = pBaseUI->GetColor().x;
	colorG = pBaseUI->GetColor().y;
	colorB = pBaseUI->GetColor().z;

	iUIType = pBaseUI->GetUIType();

	if (iUIType == UI_TEXT)
	{
		text = dynamic_cast<EngineText*>(pBaseUI)->GetText();
	}
	else
	{
		text = "";
	}
}
