#include <DirectX11BasePch.h>
#include "UI/EngineUIContainer.h"
#include "UI/EnginePanel.h"
#include "UI/EngineButton.h"
#include "UI/EngineText.h"
#include "UI/EngineSelector.h"
#include "UI/EngineSlider.h"
#include "EngineObjectManager.h"
#include "EngineAccessor.h"

EngineUIContainer::EngineUIContainer() : Object()
{
	m_pBaseUIs = new List<EngineBaseUI>();
	m_pInteractibleUIs = new List<EngineBaseUI>();
	m_pMouseEnteredBaseUI = 0;
	m_iHighlightedUIIndex = -1;
	m_iMenuType = -1;
	m_iNextUIContainer = -1;
}

void EngineUIContainer::RegisterGameClass()
{
	GET_OBJECT_MANAGER()->Register(ToStringHash("EngineUIContainer"), EngineUIContainer::Create);
}

void EngineUIContainer::OnEnter()
{

}

void EngineUIContainer::OnClick()
{

}

void EngineUIContainer::OnExit()
{

}

int EngineUIContainer::Initialize()
{
	int iRet = Object::Initialize();

	// Initialize base UIs
	for (Iterator<EngineBaseUI>* it = m_pBaseUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		iRet |= it->GetValue()->Initialize();
	}

	return iRet;
}

int EngineUIContainer::PreUpdate()
{
	int iRet = Object::PreUpdate();

	// Update base UIs
	for (Iterator<EngineBaseUI>* it = m_pBaseUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			iRet = it->GetValue()->PreUpdate();
		}
	}

	return iRet;
}

int EngineUIContainer::Update(float fDeltaTime)
{
	int iRet = Object::Update(fDeltaTime);

	// Update base UIs
	for (Iterator<EngineBaseUI>* it = m_pBaseUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			iRet = it->GetValue()->Update(fDeltaTime);
		}
	}

	return iRet;
}

int EngineUIContainer::Render()
{
	int iRet = 0;

	if (IsHidden())
	{
		return iRet;
	}

	// Update base UIs
	for (Iterator<EngineBaseUI>* it = m_pBaseUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsActive())
		{
			iRet |= it->GetValue()->Render();
		}
	}

	iRet |= Object::Render();

	return iRet;
}

int EngineUIContainer::Destroy()
{
	int iRet = Object::Destroy();

	// Destroy base UIs
	for (Iterator<EngineBaseUI>* it = m_pBaseUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		iRet = it->GetValue()->Destroy();
	}

	// Clean up objects
	m_pInteractibleUIs->Clear();
	m_pBaseUIs->Clear();
	SafeReleaseTemplate(m_pBaseUIs);

	return iRet;
}

int EngineUIContainer::OnMousePressed(const MousePressedEvent& mousePressedEvent)
{
	int iRet = -1;

	for (Iterator<EngineBaseUI>* it = m_pInteractibleUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		iRet = it->GetValue()->OnMousePressed(mousePressedEvent);

		if (iRet >= 0)
		{
			break;
		}
	}

	return iRet;
}

int EngineUIContainer::OnMouseEnter(XMFLOAT3 vMousePos)
{
	return 0;
}

int EngineUIContainer::OnMouseExit(XMFLOAT3 vMousePos)
{
	return 0;
}

int EngineUIContainer::OnSelectPrevOption()
{
	if (m_pInteractibleUIs->GetNumElements() == 0)
	{
		return 0;
	}

	if (m_iHighlightedUIIndex >= 0)
	{
		m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()->OnUnHighlighted();
	}

	m_iHighlightedUIIndex--;

	if (m_iHighlightedUIIndex < 0)
	{
		m_iHighlightedUIIndex = m_pInteractibleUIs->GetNumElements() - 1;
	}

	m_pMouseEnteredBaseUI = m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue();
	m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()->OnHighlighted();
	Application::GetInstance()->ReRender();
	return 0;
}

int EngineUIContainer::OnSelectNextOption()
{
	if (m_pInteractibleUIs->GetNumElements() == 0)
	{
		return 0;
	}

	if (m_iHighlightedUIIndex >= 0)
	{
		m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()->OnUnHighlighted();
	}

	m_iHighlightedUIIndex++;

	if (m_iHighlightedUIIndex >= 0)
	{
		if (m_iHighlightedUIIndex >= m_pInteractibleUIs->GetNumElements())
		{
			m_iHighlightedUIIndex = 0;
		}
	}

	m_pMouseEnteredBaseUI = m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue();
	m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()->OnHighlighted();
	Application::GetInstance()->ReRender();
	return 0;
}

int EngineUIContainer::OnConfirmOption()
{
	if (m_iHighlightedUIIndex >= 0)
	{
		m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()->Post();
	}
	return 0;
}

int EngineUIContainer::OnSelectRightOption()
{
	if (m_iHighlightedUIIndex >= 0)
	{
		if (dynamic_cast<EngineSelector*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()))
		{
			dynamic_cast<EngineSelector*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue())->OnSelectRightOption();
		}

		if (dynamic_cast<EngineSlider*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()))
		{
			dynamic_cast<EngineSlider*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue())->OnSelectRightOption();
		}
	}
	return 0;
}

int EngineUIContainer::OnSelectLeftOption()
{
	if (m_iHighlightedUIIndex >= 0)
	{
		if (dynamic_cast<EngineSelector*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()))
		{
			dynamic_cast<EngineSelector*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue())->OnSelectLeftOption();
		}

		if (dynamic_cast<EngineSlider*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue()))
		{
			dynamic_cast<EngineSlider*>(m_pInteractibleUIs->GetElementAtIndex(m_iHighlightedUIIndex)->GetValue())->OnSelectLeftOption();
		}
	}
	return 0;
}

bool EngineUIContainer::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	bool bRet = Object::ReadSaveData(itr);

	int iNumUI = (*itr)["NumUIs"].GetInt();
	itr++;

	for (int i = 0; i < iNumUI; i++)
	{
		int iUIType = (*itr)["UIType"].GetInt();
		const char* sUIName = (*itr)["Name"].GetString();
		float iXPos = (*itr)["X"].GetDouble();
		float iYPos = (*itr)["Y"].GetDouble();
		float iZPos = (*itr)["Z"].GetDouble();

		switch (iUIType)
		{
		case UI_PANEL:
		{
			float iSizeX = (*itr)["SizeX"].GetDouble();
			float iSizeY = (*itr)["SizeY"].GetDouble();
			float iColorR = (*itr)["ColorR"].GetDouble();
			float iColorG = (*itr)["ColorG"].GetDouble();
			float iColorB = (*itr)["ColorB"].GetDouble();
			EnginePanel* pPanel = new EnginePanel(XMFLOAT3(iXPos, iYPos, iZPos), XMFLOAT3(iSizeX, iSizeY, 0), XMFLOAT3(iColorR, iColorG, iColorB));
			pPanel->SetObjectName(_strdup(sUIName));
			AddUI(pPanel);
			pPanel->Initialize();
		}
		break;
		case UI_BUTTON:
		{
			float iSizeX = (*itr)["SizeX"].GetDouble();
			float iSizeY = (*itr)["SizeY"].GetDouble();
			float iColorR = (*itr)["ColorR"].GetDouble();
			float iColorG = (*itr)["ColorG"].GetDouble();
			float iColorB = (*itr)["ColorB"].GetDouble();
			float iHighlightColorR = (*itr)["HighlightColorR"].GetDouble();
			float iHighlightColorG = (*itr)["HighlightColorG"].GetDouble();
			float iHighlightColorB = (*itr)["HighlightColorB"].GetDouble();
			EngineButton* pButton = new EngineButton(XMFLOAT3(iXPos, iYPos, iZPos), XMFLOAT3(iSizeX, iSizeY, 0), XMFLOAT3(iColorR, iColorG, iColorB));
			pButton->SetObjectName(_strdup(sUIName));
			pButton->SetHighlightColor(XMFLOAT3(iHighlightColorR, iHighlightColorG, iHighlightColorB));
			AddUI(pButton);
			pButton->Initialize();
		}
		break;
		case UI_TEXT:
		{
			const char* sTest = (*itr)["Text"].GetString();
			EngineText* pText = new EngineText(_strdup(sTest), XMFLOAT3(iXPos, iYPos, iZPos));
			pText->SetObjectName(_strdup(sUIName));
			AddUI(pText);
			pText->Initialize();
		}
		break;
		case UI_SELECTOR:
		{
			float iSizeX = (*itr)["SizeX"].GetDouble();
			float iSizeY = (*itr)["SizeY"].GetDouble();
			float iTextX = (*itr)["TextX"].GetDouble();
			float iTextY = (*itr)["TextY"].GetDouble();
			float iTextZ = (*itr)["TextZ"].GetDouble();
			float iColorR = (*itr)["ColorR"].GetDouble();
			float iColorG = (*itr)["ColorG"].GetDouble();
			float iColorB = (*itr)["ColorB"].GetDouble();
			EngineSelector* pSelector = new EngineSelector(XMFLOAT3(iXPos, iYPos, iZPos), XMFLOAT3(iSizeX, iSizeY, 0), XMFLOAT3(iTextX, iTextY, iTextZ), XMFLOAT3(iColorR, iColorG, iColorB));
			pSelector->SetObjectName(_strdup(sUIName));
			AddUI(pSelector);
			pSelector->Initialize();
		}
		break;
		case UI_SLIDER:
		{
			float iSizeX = (*itr)["SizeX"].GetDouble();
			float iSizeY = (*itr)["SizeY"].GetDouble();
			float iTextX = (*itr)["TextX"].GetDouble();
			float iTextY = (*itr)["TextY"].GetDouble();
			float iTextZ = (*itr)["TextZ"].GetDouble();
			float iValue = (*itr)["Value"].GetDouble();
			float iIncreaseStep = (*itr)["IncreaseStep"].GetDouble();
			float iMinValue = (*itr)["MinValue"].GetDouble();
			float iMaxValue = (*itr)["MaxValue"].GetDouble();
			float iColorR = (*itr)["ColorR"].GetDouble();
			float iColorG = (*itr)["ColorG"].GetDouble();
			float iColorB = (*itr)["ColorB"].GetDouble();
			EngineSlider* pSelector = new EngineSlider(XMFLOAT3(iXPos, iYPos, iZPos), XMFLOAT3(iSizeX, iSizeY, 0), XMFLOAT3(iTextX, iTextY, iTextZ), iValue, iIncreaseStep, iMinValue, iMaxValue, XMFLOAT3(iColorR, iColorG, iColorB));
			pSelector->SetObjectName(_strdup(sUIName));
			AddUI(pSelector);
			pSelector->Initialize();
		}
		break;
		}

		itr++;
	}

	return bRet;
}

bool EngineUIContainer::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	bool bRet = Object::WriteSaveData(writer);

	// Write Save Data to UI Container Data
	writer->StartObject();
	writer->Key("NumUIs");
	writer->Int(m_pBaseUIs->GetNumElements());
	writer->EndObject();

	for (Iterator<EngineBaseUI>* it = m_pBaseUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		writer->StartObject();
		writer->Key("UIType");
		writer->Int(it->GetValue()->GetUIType());

		writer->Key("X");
		writer->Double(it->GetValue()->GetWorldPosition().x);

		writer->Key("Y");
		writer->Double(it->GetValue()->GetWorldPosition().y);

		writer->Key("Z");
		writer->Double(it->GetValue()->GetWorldPosition().z);

		switch (it->GetValue()->GetUIType())
		{
		case UI_PANEL:
		{
			writer->Key("SizeX");
			writer->Double(it->GetValue()->GetSize().x);
			writer->Key("SizeY");
			writer->Double(it->GetValue()->GetSize().y);
			writer->Key("ColorR");
			writer->Double(it->GetValue()->GetColor().x);
			writer->Key("ColorG");
			writer->Double(it->GetValue()->GetColor().y);
			writer->Key("ColorB");
			writer->Double(it->GetValue()->GetColor().z);
		}
		break;
		case UI_BUTTON:
		{
			writer->Key("SizeX");
			writer->Double(it->GetValue()->GetSize().x);
			writer->Key("SizeY");
			writer->Double(it->GetValue()->GetSize().y);
			writer->Key("ColorR");
			writer->Double(it->GetValue()->GetColor().x);
			writer->Key("ColorG");
			writer->Double(it->GetValue()->GetColor().y);
			writer->Key("ColorB");
			writer->Double(it->GetValue()->GetColor().z);
		}
		break;
		case UI_TEXT:
		{
			writer->Key("Text");
			writer->String(dynamic_cast<EngineText*>(it->GetValue())->GetText());
		}
		break;
		}

		writer->EndObject();
	}

	return bRet;
}

EngineBaseUI* EngineUIContainer::GetUIByIndex(int iIndex)
{
	return m_pBaseUIs->GetElementAtIndex(iIndex)->GetValue();
}

EngineBaseUI* EngineUIContainer::GetUIByName(const char* sUIName)
{
	for (Iterator<EngineBaseUI>* it = m_pBaseUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		if (strcmp(it->GetValue()->GetObjectName(), sUIName) == 0)
		{
			return it->GetValue();
		}
	}

	return 0;
}

bool EngineUIContainer::IsInUISpace(XMFLOAT3 vMousePos)
{
	int iBaseUIIndex = 0;

	if (m_pMouseEnteredBaseUI && m_pMouseEnteredBaseUI->IsInUISpace(vMousePos))
	{
		return true;
	}

	for (Iterator<EngineBaseUI>* it = m_pInteractibleUIs->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->IsInUISpace(vMousePos) && m_iHighlightedUIIndex != iBaseUIIndex)
		{
			if (m_pMouseEnteredBaseUI && !m_pMouseEnteredBaseUI->IsMouseExit())
			{
				m_pMouseEnteredBaseUI->OnMouseExit(vMousePos);
			}
			m_iHighlightedUIIndex = iBaseUIIndex;
			m_pMouseEnteredBaseUI = it->GetValue();

			if (!m_pMouseEnteredBaseUI->IsMouseEntered())
			{
				m_pMouseEnteredBaseUI->OnMouseEnter(vMousePos);
			}
			return true;
		}
		iBaseUIIndex++;
	}

	if (m_pMouseEnteredBaseUI && !m_pMouseEnteredBaseUI->IsMouseExit())
	{
		m_pMouseEnteredBaseUI->OnMouseExit(vMousePos);
	}
	m_iHighlightedUIIndex = -1;
	m_pMouseEnteredBaseUI = 0;

	return false;
}

void EngineUIContainer::TestUIContainer()
{
	EngineButton* pButton = new EngineButton(XMFLOAT3(10, -0.3, 0), XMFLOAT3(2, 1, 6), XMFLOAT3(1, 1, 0));
	//pButton->SetHighlightColor(XMFLOAT3(0, 1, 0));
	AddUI(pButton);

	EnginePanel* pPanel = new EnginePanel(XMFLOAT3(10, 0, 0), XMFLOAT3(6, 2, 6));
	pPanel->SetHighlightColor(XMFLOAT3(0, 1, 0));
	AddUI(pPanel);

	EngineText* pText = new EngineText("Hello World", XMFLOAT3(13, -1.0, 0));
	AddUI(pText);
}

