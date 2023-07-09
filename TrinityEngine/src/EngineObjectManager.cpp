#include <DirectX11BasePch.h>
#include "EngineObjectManager.h"

EngineObjectManager* EngineObjectManager::instance = 0;
EngineObjectManager* EngineObjectManager::GetInstance()
{
	if (!instance)
	{
		instance = new EngineObjectManager();
	}

	return instance;
}

void EngineObjectManager::Register(int iClassNameHash, CreateFuncType classCreateFunc)
{
	Assert(m_Observers.find(iClassNameHash) == m_Observers.end());
	m_Observers[iClassNameHash] = classCreateFunc;
}

Object* EngineObjectManager::CreateObjectBySource(const char* sObjectSource)
{
	if (m_pObjectData.DeserializeFromFile(sObjectSource))
	{
		m_pObjectData.m_pObject->SetObjectSource(sObjectSource);
		return m_pObjectData.m_pObject;
	}

	return 0;
}

Object* EngineObjectManager::CreateObjectByClassNameHash(int iClassNameHash)
{
	Assert(m_Observers.find(iClassNameHash) != m_Observers.end());
	return m_Observers[iClassNameHash]();
}

void EngineObjectManager::SaveObject(Object* pObject)
{
	m_pObjectData.m_pObject = pObject;
	m_pObjectData.SerializeToFile(pObject->GetObjectSource());
}

EngineObjectManager::EngineObjectManager()
{

}

bool ObjectData::Deserialize(rapidjson::Value::ConstValueIterator& itr)
{
	const rapidjson::Value& obj = (*itr);

	// Read the object class name first
	const char* className = obj["ObjectClassName"].GetString();
	m_pObject = EngineObjectManager::GetInstance()->CreateObjectByClassNameHash(ToStringHash(className));
	itr++;

	return m_pObject->ReadSaveData(itr);
}

bool ObjectData::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
	Assert(m_pObject);
	return m_pObject->WriteSaveData(writer);
}
