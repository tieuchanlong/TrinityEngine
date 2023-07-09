#pragma once
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include <string>

// References serialize and deserialize objects: https://gotask.net/programming/serialize-and-deserialize-object-in-cpp-using-rapidjson/
// References serialize and deserialize array: https://gotask.net/programming/serialize-and-deserialize-array-in-cpp-using-rapidjson/

class SaveData
{
public:
	SaveData();
	
	virtual bool InitDocument(const char* sFileName, rapidjson::Document& doc);

	virtual const std::string Serialize();

	virtual bool DeserializeFromFile(const char* sFileName);
	virtual bool SerializeToFile(const char* sFileName);

	virtual bool Deserialize(rapidjson::Value::ConstValueIterator& itr) = 0;
	virtual bool InitializeSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const = 0;
	virtual bool EndSerialize(rapidjson::Writer<rapidjson::StringBuffer>* writer);
protected:
};