#include <DirectX11BasePch.h>
#include "EngineLevelManager.h"
#include "EngineObjectManager.h"
#include "EngineAccessor.h"

int EngineLevelManager::Initialize()
{
	int iRet = 0;

	// Initialize objects
	for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			iRet |= it->GetValue()->Initialize();
		}
	}

	EngineEventManager::GetInstance()->SubscribeFunction(EventType::CancelPressed, std::bind(&EngineLevelManager::OnCancelPressed, this, std::placeholders::_1));

	return iRet;
}

int EngineLevelManager::PreUpdate()
{
	int iRet = 0;

	// PreUpdate objects
	for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			iRet |= it->GetValue()->PreUpdate();
		}
	}

	return 0;
}

int EngineLevelManager::Update(float fDeltaTime)
{
	int iRet = 0;

	// If the transition menu is on top, all the levels should be hidden
	if (GET_GUI_MANAGER()->GetUIByType(TRANSITION_MENU))
	{
		m_bTransitionMenuActive = true;
		for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
		{
			if (it->GetValue()->IsActive())
			{
				it->GetValue()->SetHidden(true);
			}
		}
	}
	else if (m_bTransitionMenuActive)
	{
		m_bTransitionMenuActive = false;

		for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
		{
			if (it->GetValue()->IsActive())
			{
				it->GetValue()->SetHidden(false);
			}
		}
	}

	// Update objects
	for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			iRet |= it->GetValue()->Update(fDeltaTime);
		}
	}

	return iRet;
}

int EngineLevelManager::Render()
{
	int iRet = 0;

	// Render levels
	for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			iRet |= it->GetValue()->Render();
		}
	}

	return iRet;
}

int EngineLevelManager::Destroy()
{
	int iRet = 0;

	// Destroy levels
	for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Destroy();
	}

	// Clean up levels
	m_pLevels->Clear();
	SafeReleaseTemplate(m_pLevels);

	return iRet;
}

Level* EngineLevelManager::CreateLevel(const char* sLevelPath)
{
	if (m_pLevelData.DeserializeFromFile(sLevelPath))
	{
		m_pLevelData.m_pLevel->SetObjectSource(_strdup(sLevelPath));
		return m_pLevelData.m_pLevel;
	}

	return 0;
}

void EngineLevelManager::AddLevel(Level* pLevel)
{
	m_pCurLevel = pLevel;
	m_pLevels->Add(m_pCurLevel);
	m_pCurLevel->SetLevelIndex(m_pLevels->GetNumElements() - 1);
	m_pCurLevel->Initialize();
}

void EngineLevelManager::SaveLevel(Level* pLevel)
{
	m_pLevelData.m_pLevel = pLevel;
	m_pLevelData.SerializeToFile(pLevel->GetObjectSource());
}

bool EngineLevelManager::IsDoneLoadingLevels()
{
	bool bIsDoneLoadingLevels = true;

	for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			bIsDoneLoadingLevels &= it->GetValue()->IsDoneLoadingObjects();
		}
	}

	return bIsDoneLoadingLevels;
}

void EngineLevelManager::OnCancelPressed(const Event& e)
{
	for (Iterator<Level>* it = m_pLevels->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			it->GetValue()->OnCancelPressed();
		}
	}
}

EngineLevelManager::EngineLevelManager()
{
	m_pLevels = new List<Level>();
	m_pCurLevel = 0;
}

bool LevelData::Deserialize(rapidjson::Value::ConstValueIterator& itr)
{
	const rapidjson::Value& obj = (*itr);

	// Read the object class name first
	const char* className = obj["ObjectClassName"].GetString();
	m_pLevel = (Level*)GET_OBJECT_MANAGER()->CreateObjectByClassNameHash(ToStringHash(className));
	itr++;

	return m_pLevel->ReadSaveData(itr);
}

bool LevelData::Serialize(rapidjson::Writer<rapidjson::StringBuffer>* writer) const
{
	return m_pLevel->WriteSaveData(writer);
}
