#include <DirectX11BasePch.h>
#include "EngineObject.h"
#include "EngineRenderComponent.h"
#include "Physics/EngineRigidBodyComponent.h"
#include "EngineObjectManager.h"
#include "EngineComponentManager.h"
#include "EngineAccessor.h"

#include <string>

int Object::m_iNumObjects = 0;
Object::Object()
{
	m_pComponents = new List<Component>();
	m_bActive = true;
	m_bHidden = false;

	m_sObjectName = new char();
	snprintf(m_sObjectName, 256, "Object%i", m_iNumObjects + 1);
	m_iNumObjects++;
	m_bIsInitialized = false;
}

void Object::RegisterGameClass()
{
	GET_OBJECT_MANAGER()->Register(ToStringHash("Object"), Object::Create);
}

bool Object::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	const rapidjson::Value& obj = (*itr);
	int iNumComponents = obj["NumComponents"].GetInt();
	itr++;

	for (int i = 0; i < iNumComponents; i++)
	{
		const char* sComponentClassName = (*itr)["ComponentClassName"].GetString();
		Component* pComponent = EngineComponentManager::GetInstance()->CreateComponent(ToStringHash(sComponentClassName));
		itr++;
		pComponent->ReadSaveData(itr);
		pComponent->SetOwner(this);
		AddComponent(pComponent);
	}

	return true;
}

bool Object::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
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

	return true;
}

int Object::Initialize()
{
	m_bIsInitialized = true;

	// Initialize components
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Initialize();
	}

	return 0;
}

int Object::PreUpdate()
{
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			it->GetValue()->PreUpdate();
		}
	}

	return 0;
}

int Object::Update(float fDeltaTime)
{
	if (GetComponentByType<EngineRigidComponent>())
	{
		XMFLOAT3 vSpeed = GetComponentByType<EngineRigidComponent>()->GetSpeed();

		if (vSpeed.x != 0 || vSpeed.y != 0 || vSpeed.z != 0)
		{
			m_vPos = m_vPos + fDeltaTime * vSpeed;

			for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
			{
				if (dynamic_cast<EngineRenderComponent*>(it->GetValue()))
				{
					dynamic_cast<EngineRenderComponent*>(it->GetValue())->UpdateTranslation(XMFLOAT3(vSpeed.x * fDeltaTime, vSpeed.y * fDeltaTime, vSpeed.z * fDeltaTime));
				}
			}
		}
	}

	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			it->GetValue()->Update(fDeltaTime);
		}
	}

	return 0;
}

int Object::Destroy()
{
	// Clear the component list
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Destroy();
	}

	m_pComponents->Clear();
	SafeReleaseTemplate(m_pComponents);

	return 0;
}

int Object::Render()
{
	int iRet = 0;

	if (IsHidden())
	{
		return iRet;
	}

	// Initialize components
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			it->GetValue()->Render();
		}
	}

	return iRet;
}

void Object::SetActive(bool bActive)
{
	if (bActive == m_bActive)
	{
		return;
	}

	m_bActive = bActive;

	// Activate/ Deactivate components
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		if (!dynamic_cast<EngineRenderComponent*>(it->GetValue()))
		{
			it->GetValue()->SetActive(m_bActive);
		}
	}
}

void Object::SetHidden(bool bHidden)
{
	if (bHidden == m_bHidden)
	{
		return;
	}

	m_bHidden = bHidden;

	// Activate/ Deactivate render components
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		if (dynamic_cast<EngineRenderComponent*>(it->GetValue()))
		{
			dynamic_cast<EngineRenderComponent*>(it->GetValue())->SetActive(m_bActive);
		}
	}

	Application::GetInstance()->ReRender();
}

void Object::AddComponent(Component* component)
{
	if (dynamic_cast<EngineRenderComponent*>(component))
	{
		m_pRenderComponent = dynamic_cast<EngineRenderComponent*>(component);
	}

	component->SetOwner(this);
	m_pComponents->Add(component);
}


DirectX::XMFLOAT3 Object::GetWorldPosition()
{
	return m_vPos;
}

DirectX::XMFLOAT3 Object::GetSize()
{
	EngineRenderComponent* pRenderComp = GetComponentByType<EngineRenderComponent>();

	if (pRenderComp)
	{
		return pRenderComp->GetSize();
	}

	return XMFLOAT3(0, 0, 0);
}

DirectX::XMFLOAT3 Object::GetColor()
{
	EngineRenderComponent* pRenderComp = GetComponentByType<EngineRenderComponent>();

	if (pRenderComp)
	{
		return pRenderComp->GetColor();
	}

	return XMFLOAT3(0, 0, 0);
}

bool Object::HasRenderComponent()
{
	return GetComponentByType<EngineRenderComponent>() != NULL;
}

bool Object::HasRigidBodyComponent()
{
	return GetComponentByType<EngineRigidComponent>() != NULL;
}

void Object::OnButtonPressed()
{
	// Test turn off active
	SetActive(false);
}

AABB Object::GetBoundingDimension()
{
	for (Iterator<Component>* it = m_pComponents->GetHead(); it != NULL; it = it->GetNext())
	{
		if (dynamic_cast<EngineRenderComponent*>(it->GetValue()))
		{
			m_AABB.minX = m_vPos.x + dynamic_cast<EngineRenderComponent*>(it->GetValue())->GetMinDim().x;
			m_AABB.maxX = m_vPos.x + dynamic_cast<EngineRenderComponent*>(it->GetValue())->GetMaxDim().x;
			m_AABB.minY = m_vPos.y + dynamic_cast<EngineRenderComponent*>(it->GetValue())->GetMinDim().y;
			m_AABB.maxY = m_vPos.y + dynamic_cast<EngineRenderComponent*>(it->GetValue())->GetMaxDim().y;
			m_AABB.minZ = m_vPos.z + dynamic_cast<EngineRenderComponent*>(it->GetValue())->GetMinDim().z;
			m_AABB.maxZ = m_vPos.z + dynamic_cast<EngineRenderComponent*>(it->GetValue())->GetMaxDim().z;
		}
	}

	return m_AABB;
}

void Object::TestComponents()
{
	// Test the engine render component
	
}
