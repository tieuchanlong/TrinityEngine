#include <DirectX11BasePch.h>
#include "EngineComponent.h"
#include "EngineComponentManager.h"

Component::Component()
{
	m_bActive = true;
}

void Component::RegisterGameClass()
{
	EngineComponentManager::GetInstance()->Register(ToStringHash("Component"), Component::Create);
}

bool Component::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	return true;
}

bool Component::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	writer->StartObject();
	writer->Key("ComponentClassName");
	writer->String(GetComponentClassName());
	writer->EndObject();

	return true;
}

int Component::Initialize()
{
	return 0;
}

int Component::PreUpdate()
{
	return 0;
}

int Component::Update(float fDeltaTime)
{
	return 0;
}

int Component::Destroy()
{
	return 0;
}

int Component::Render()
{
	return 0;
}

void Component::SetActive(bool bActive)
{
	if (m_bActive == bActive)
	{
		return;
	}

	m_bActive = bActive;
}
