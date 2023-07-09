#include <DirectX11BasePch.h>
#include "SaveData/EngineSaveData.h"
#include "rapidjson/filereadstream.h"
#include <fstream>

#define MAX_FILE_BUFFER 15000

SaveData::SaveData()
{

}

bool SaveData::InitDocument(const char* sFileName, rapidjson::Document& doc)
{
	if (sFileName == NULL && sFileName[0] == '\0')
	{
		return false;
	}

	FILE* fp; 
	fopen_s(&fp, sFileName, "rb"); // non-Windows use "r"

	if (fp == NULL)
	{
		return false;
	}

	char readBuffer[MAX_FILE_BUFFER];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	doc.ParseStream(is);

	fclose(fp);
	return true;
}

bool SaveData::InitializeSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	writer->StartArray();
	return true;
}

const std::string SaveData::Serialize()
{
	rapidjson::StringBuffer ss;
	rapidjson::Writer<rapidjson::StringBuffer> writer(ss);		// Can also use Writer for condensed formatting

	InitializeSerialize(&writer);
	if (Serialize(&writer))
	{
		EndSerialize(&writer);
		return ss.GetString();
	}
	return "";
}

bool SaveData::EndSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	writer->EndArray();
	return true;
}

bool SaveData::DeserializeFromFile(const char* sFileName)
{
	rapidjson::Document doc;
	if (!InitDocument(sFileName, doc))
	{
		return false;
	}

	rapidjson::Value::ConstValueIterator itr = doc.Begin();

	if (itr->IsNull())
	{
		return false;
	}

	Deserialize(itr);

	return true;
}

bool SaveData::SerializeToFile(const char* sFileName)
{
	std::ofstream f(sFileName);
	std::string s = Serialize();
	f << s;
	f.flush();
	f.close();

	return true;
}
