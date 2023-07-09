#include <DirectX11BasePch.h>
#include "ObjectSpawner.h"
#include "EngineObjectManager.h"
#include "EngineAccessor.h"
#include "EngineCubeComponent.h"

#define MAX_RANDOMIZATION 100

ObjectSpawner::ObjectSpawner() : Object()
{
	m_pSpawnObjs = new List<Object>();
	m_pSpawnRegionHighlights = new List<Object>();
	m_iCurSpawnObjIndex = 0;
	m_bCanSpawnObjects = true;
	m_bShowHighlights = false;
	m_pLevelOwner = 0;
	Quantity = 0;
}

void ObjectSpawner::RegisterGameClass()
{
	GET_OBJECT_MANAGER()->Register(ToStringHash("ObjectSpawner"), ObjectSpawner::Create);
}

bool ObjectSpawner::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	bool bRet = Object::ReadSaveData(itr);

	Quantity = (*itr)["Quantity"].GetInt();
	m_iSpawnObjectSourceCount = (*itr)["NumObjectSource"].GetInt();
	MinX = (*itr)["MinX"].GetInt();
	MaxX = (*itr)["MaxX"].GetInt();
	MinY = (*itr)["MinY"].GetInt();
	MaxY = (*itr)["MaxY"].GetInt();
	MinZ = (*itr)["MinZ"].GetInt();
	MaxZ = (*itr)["MaxZ"].GetInt();
	itr++;

	for (int i = 0; i < m_iSpawnObjectSourceCount; i++)
	{
		m_sSpawnObjectSources[i] = _strdup((*itr)["ObjectSource"].GetString());
		itr++;
	}

	m_iCurHighlightX = MinX;
	m_iCurHighlightY = MinY;
	m_iCurHighlightZ = MinZ;

	return bRet;
}

bool ObjectSpawner::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	bool bRet = Object::WriteSaveData(writer);

	writer->StartObject();
	writer->Key("Quantity");
	writer->Int(Quantity);
	writer->Key("NumObjectSource");
	writer->Int(m_iSpawnObjectSourceCount);
	writer->Key("MinX");
	writer->Int(MinX);
	writer->Key("MaxX");
	writer->Int(MaxX);
	writer->Key("MinY");
	writer->Int(MinY);
	writer->Key("MaxY");
	writer->Int(MaxY);
	writer->Key("MinZ");
	writer->Int(MinZ);
	writer->Key("MaxZ");
	writer->Int(MaxZ);
	writer->EndObject();

	for (int i = 0; i < m_iSpawnObjectSourceCount; i++)
	{
		writer->StartObject();
		writer->Key("ObjectSource");
		writer->String(m_sSpawnObjectSources[i]);
		writer->EndObject();
	}

	return bRet;
}

void ObjectSpawner::SpawnObjects()
{
	if (!m_bCanSpawnObjects || !m_pLevelOwner)
	{
		return;
	}

	for (int i = 0; i < MAX_LOADING_OBJECTS_PER_FRAME; ++i)
	{
		if (m_iCurSpawnObjIndex >= Quantity)
		{
			return;
		}

		m_iCurSpawnObjIndex++;

		int iRandObj = rand() % m_iSpawnObjectSourceCount;
		int iRandX = (m_pLevelOwner->GetWorldPosition() + m_vPos).x + MinX + ((MaxX - MinX != 0) ? rand() % (MaxX - MinX) : 0);
		int iRandY = (m_pLevelOwner->GetWorldPosition() + m_vPos).y + MinY + ((MaxY - MinY != 0) ? rand() % (MaxY - MinY) : 0);
		int iRandZ = (m_pLevelOwner->GetWorldPosition() + m_vPos).z + MinZ + ((MaxZ - MinZ != 0) ? rand() % (MaxZ - MinZ) : 0);

		char fullPath[256] = { 0 };
		snprintf(fullPath, 256, "%s%s", Application::GetInstance()->GetCurDirectory(), m_sSpawnObjectSources[iRandObj]);

		Object* pObj = GET_OBJECT_MANAGER()->CreateObjectBySource(fullPath);
		int iCurRandCOunt = 0;

		while (true)
		{
			if (iCurRandCOunt >= MAX_RANDOMIZATION)
			{
				break;
			}

			pObj->SetWorldPosition(XMFLOAT3(iRandX, iRandY, iRandZ));
			EngineBoundingBoxComponent* pCurObjBoundingBox = pObj->GetComponentByType<EngineBoundingBoxComponent>();

			bool bCollidedWithOtherObjs = false;

			// Update objects
			for (Iterator<Object>* it = m_pSpawnObjs->GetHead(); it != NULL; it = it->GetNext())
			{
				if (it->GetValue()->IsActive())
				{
					EngineBoundingBoxComponent* pBoundingBox = it->GetValue()->GetComponentByType<EngineBoundingBoxComponent>();
					if (pBoundingBox && pCurObjBoundingBox->GetAABB().IsColliding(pBoundingBox->GetAABB()))
					{
						bCollidedWithOtherObjs = true;
						break;
					}
				}
			}

			if (!bCollidedWithOtherObjs)
			{
				break;
			}

			iCurRandCOunt++;
		}

		if (iCurRandCOunt >= MAX_RANDOMIZATION)
		{
			pObj->Destroy();
			SafeReleaseTemplate(pObj);
			pObj = NULL;
		}
		else
		{
			m_pLevelOwner->AddObject(pObj);
			m_pSpawnObjs->Add(pObj);
		}
	}
}

void ObjectSpawner::SpawnHighlights()
{
	for (int i = 0; i < MAX_LOADING_OBJECTS_PER_FRAME; ++i)
	{
		char fullHighlightCubePath[256] = { 0 };
		snprintf(fullHighlightCubePath, 256, "%s\\common\\GameObjects\\HighlightCube.json", Application::GetInstance()->GetCurDirectory());

		Object* pHighlight = GET_OBJECT_MANAGER()->CreateObjectBySource(fullHighlightCubePath);
		pHighlight->GetComponentByType<EngineCubeComponent>()->SetAlpha(0.05f);
		pHighlight->SetWorldPosition(XMFLOAT3((m_pLevelOwner->GetWorldPosition() + m_vPos).x + m_iCurHighlightX, (m_pLevelOwner->GetWorldPosition() + m_vPos).y + m_iCurHighlightY, (m_pLevelOwner->GetWorldPosition() + m_vPos).z + m_iCurHighlightZ));
		pHighlight->SetHidden(!m_bShowHighlights);
		m_pLevelOwner->AddObject(pHighlight);
		m_pSpawnRegionHighlights->Add(pHighlight);

		m_iCurHighlightZ++;

		if (m_iCurHighlightZ > MaxZ)
		{
			m_iCurHighlightZ = MinZ;
			m_iCurHighlightY++;

			if (m_iCurHighlightY > MaxY)
			{
				m_iCurHighlightY = MinY;
				m_iCurHighlightX++;

				if (m_iCurHighlightX > MaxX)
				{
					return;
				}
			}
		}
	}
}

int ObjectSpawner::PreUpdate()
{
	int iRet = Object::PreUpdate();

	if (m_iCurSpawnObjIndex < Quantity)
	{
		SpawnObjects();
	}

	if (m_iCurHighlightX <= MaxX)
	{
		SpawnHighlights();
	}

	return iRet;
}

int ObjectSpawner::Destroy()
{
	int iRet = Object::Destroy();

	m_pSpawnObjs->Clear();
	m_pSpawnRegionHighlights->Clear();

	SafeReleaseTemplate(m_pSpawnObjs);
	SafeReleaseTemplate(m_pSpawnRegionHighlights);

	return iRet;
}

void ObjectSpawner::ShowSpawnRegionHighlights(bool bShowHighlight)
{
	if (bShowHighlight == m_bShowHighlights)
	{
		return;
	}

	m_bShowHighlights = bShowHighlight;

	for (Iterator<Object>* it = m_pSpawnRegionHighlights->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->SetHidden(!m_bShowHighlights);
	}
}
