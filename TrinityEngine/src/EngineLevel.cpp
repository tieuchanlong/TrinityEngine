#include <DirectX11BasePch.h>
#include "EngineLevel.h"
#include "EngineCubeComponent.h"
#include "Physics/EngineRigidBodyComponent.h"
#include "Physics/EngineBoundingBoxComponent.h"
#include "EngineObjectManager.h"
#include "EngineLevelManager.h"
#include "EngineAccessor.h"
#include "ObjectSpawner.h"

Level::Level() : Object()
{
	// Initialize object list stuff
	m_pObjects = new List<Object>();
	m_pObjectTempData = new List<ObjectTemporaryData>();
	m_bLoadingObjects = false;
	m_iCurLoadingObjIndex = -1;
	m_iLevelIndex = -1;
}

void Level::RegisterGameClass()
{
	GET_OBJECT_MANAGER()->Register(GetObjectClassNameHash(), Level::Create);
}

bool Level::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	bool bRet = Object::ReadSaveData(itr);

	const rapidjson::Value& obj = (*itr);
	int iNumObjects = obj["NumObjects"].GetInt();
	itr++;

	for (int i = 0; i < iNumObjects; i++)
	{
		ObjectTemporaryData* tempData = new ObjectTemporaryData();
		tempData->vPos.x = (*itr)["X"].GetDouble();
		tempData->vPos.y = (*itr)["Y"].GetDouble();
		tempData->vPos.z = (*itr)["Z"].GetDouble();
		tempData->sObjectSource = _strdup((*itr)["ObjectSource"].GetString());
		m_pObjectTempData->Add(tempData);
		itr++;
	}

	m_iCurLoadingObjIndex = 0;
	m_bLoadingObjects = true;
	return bRet;
}

bool Level::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	// Write the position and number of component first
	writer->StartObject();

	writer->Key("ObjectClassName");
	writer->String(GetObjectClassName());

	writer->EndObject();

	writer->StartObject();

	writer->Key("NumComponents");
	writer->Int(m_pComponents->GetNumElements());

	writer->EndObject();

	// Save components
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->WriteSaveData(writer);
	}

	writer->StartObject();
	writer->Key("NumObjects");
	writer->Int(m_pObjects->GetNumElements());
	writer->EndObject();

	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		writer->StartObject();
		writer->Key("X");
		writer->Double(it->GetValue()->GetWorldPosition().x);
		writer->Key("Y");
		writer->Double(it->GetValue()->GetWorldPosition().y);
		writer->Key("Z");
		writer->Double(it->GetValue()->GetWorldPosition().z);
		writer->Key("ObjectSource");
		writer->String(it->GetValue()->GetObjectSource());
		writer->EndObject();
	}
	return true;
}

Object* Level::GetObjectByIndex(int iIndex, bool bActive /*= true*/)
{
	int iCurIndex = 0;

	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive() == bActive)
		{
			if (iCurIndex == iIndex)
			{
				return it->GetValue();
			}

			iCurIndex++;
		}
	}

	return 0;
}

Object* Level::GetCollidingObject(Object* pObject)
{
	EngineBoundingBoxComponent* pCurObjBoundingBox = pObject->GetComponentByType<EngineBoundingBoxComponent>();

	if (!pCurObjBoundingBox)
	{
		return 0;
	}

	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive() && it->GetValue() != pObject)
		{
			EngineBoundingBoxComponent* pBoundingBox = it->GetValue()->GetComponentByType<EngineBoundingBoxComponent>();
			if (pBoundingBox && pCurObjBoundingBox->GetAABB().IsColliding(pBoundingBox->GetAABB()))
			{
				return it->GetValue();
			}
		}
	}

	return 0;
}

int Level::Initialize()
{
	int iRet = Object::Initialize();

	// Initialize the object list
	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Initialize();
	}

	return iRet;
}

int Level::PreUpdate()
{
	int iRet = Object::PreUpdate();

	if (!IsDoneLoadingObjects())
	{
		LoadObjects();
	}

	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		iRet |= it->GetValue()->PreUpdate();
	}

	return iRet;
}

int Level::Update(float fDeltaTime)
{
	int iRet = Object::Update(fDeltaTime);

	// Update objects
	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		iRet |= it->GetValue()->Update(fDeltaTime);
	}

	return iRet;
}

int Level::Render()
{
	int iRet = Object::Render();

	if (IsHidden())
	{
		return iRet;
	}

	// Render objects
	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Render();
	}

	return iRet;
}

int Level::Destroy()
{
	int iRet = Object::Destroy();

	// Destroy objects
	for (Iterator<Object>* it = m_pObjects->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Destroy();
	}

	// Clean up objects
	m_pObjects->Clear();
	SafeReleaseTemplate(m_pObjects);

	return iRet;
}

void Level::LoadObjects()
{
	for (int i = 0; i < MAX_LOADING_OBJECTS_PER_FRAME; i++)
	{
		ObjectTemporaryData* pObjectData = m_pObjectTempData->GetElementAtIndex(m_iCurLoadingObjIndex)->GetValue();

		char fullPath[256] = { 0 };
		snprintf(fullPath, 256, "%s%s", Application::GetInstance()->GetCurDirectory(), pObjectData->sObjectSource);

		Object* pObject = GET_OBJECT_MANAGER()->CreateObjectBySource(pObjectData->sObjectSource);
		pObject->SetWorldPosition(pObjectData->vPos);
		pObject->SetObjectSource(_strdup(fullPath));
		AddObject(pObject);
		m_iCurLoadingObjIndex++;

		if (IsDoneLoadingObjects())
		{
			m_bLoadingObjects = false;
			return;
		}
	}
}

void Level::AddObject(Object* pObject)
{
	if (dynamic_cast<ObjectSpawner*>(pObject))
	{
		dynamic_cast<ObjectSpawner*>(pObject)->SetLevelOwner(this);
	}

	pObject->Initialize();
	m_pObjects->Add(pObject);
}

void Level::SetHidden(bool bHidden)
{
	Object::SetHidden(bHidden);

	if (bHidden == m_bHidden)
	{
		return;
	}
}

void Level::OnResumeGame()
{

}

void Level::OnCancelPressed()
{

}
