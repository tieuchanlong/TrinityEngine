#pragma once

#include <functional>
#include <map>
#include "EngineObject.h"

class ObjectData : public SaveData
{
	friend class EngineObjectManager;

public:
	virtual bool Deserialize(rapidjson::Value::ConstValueIterator& itr);
	virtual bool Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const;

protected:
	Object* m_pObject;
};

class EngineObjectManager
{
	friend class ObjectData;

public:
	using CreateFuncType = std::function<Object*()>;

	static EngineObjectManager* GetInstance();
	virtual void Register(int iClassNameHash, CreateFuncType classCreateFunc);
	virtual Object* CreateObjectBySource(const char* sObjectSource);
	virtual Object* CreateObjectByClassNameHash(int iClassNameHash);

	virtual void SaveObject(Object* pObject);
protected:
	static EngineObjectManager* instance;
	EngineObjectManager();

	std::map< int, CreateFuncType > m_Observers;

	ObjectData m_pObjectData;
};