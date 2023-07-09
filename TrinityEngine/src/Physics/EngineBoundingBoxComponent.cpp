#include <DirectX11BasePch.h>
#include "Physics/EngineBoundingBoxComponent.h"
#include "EngineObject.h"
#include "EngineComponentManager.h"

EngineBoundingBoxComponent::EngineBoundingBoxComponent(bool bIsFit /*= true*/) : Component()
{
	m_bIsFit = bIsFit;
}

void EngineBoundingBoxComponent::RegisterGameClass()
{
	EngineComponentManager::GetInstance()->Register(ToStringHash("EngineBoundingBoxComponent"), EngineBoundingBoxComponent::Create);
}

bool EngineBoundingBoxComponent::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	bool bRet = Component::ReadSaveData(itr);

	const rapidjson::Value& obj = (*itr);

	m_bIsFit = obj["IsFit"].GetBool();
	itr++;

	return bRet;
}

bool EngineBoundingBoxComponent::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	bool bRet = Component::WriteSaveData(writer);

	writer->StartObject();
	writer->Key("IsFit");
	writer->Bool(m_bIsFit);
	writer->EndObject();

	return true;
}

int EngineBoundingBoxComponent::Initialize()
{
	int iRet = Component::Initialize();

	return iRet;
}

int EngineBoundingBoxComponent::Update(float fDeltaTime)
{
	int iRet = Component::Update(fDeltaTime);

	return iRet;
}

AABB EngineBoundingBoxComponent::GetAABB()
{
	if (m_bIsFit)
	{
		m_AABB = GetOwner()->GetBoundingDimension();
	}

	return m_AABB;
}

