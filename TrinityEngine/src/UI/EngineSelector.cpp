#include <DirectX11BasePch.h>
#include "UI/EngineSelector.h"

EngineSelector::EngineSelector(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vTextPos) : EnginePanel(vPos, vSize)
{
	m_iNumOptions = 0;
	m_iCurOption = 0;
	m_iPrevOption = -1;

	pOptionText = new EngineTextComponent("", vTextPos);
	AddComponent(pOptionText);
}

EngineSelector::EngineSelector(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vTextPos, XMFLOAT3 vColor) : EnginePanel(vPos, vSize, vColor)
{
	m_iNumOptions = 0;
	m_iCurOption = 0;
	m_iPrevOption = -1;

	pOptionText = new EngineTextComponent("", vPos + vTextPos);
	AddComponent(pOptionText);
}

void EngineSelector::Subscribe(const SelectorFuncType& newFunc)
{
	selectorDispatcher.push_back(newFunc);
}

void EngineSelector::UnSubscribe(const SelectorFuncType& btnFunc)
{
	
}

void EngineSelector::OnSelectRightOption()
{
	m_iPrevOption = m_iCurOption;
	m_iCurOption++;
	m_iCurOption = (m_iCurOption + m_iNumOptions) % m_iNumOptions;

	char optionText[100] = { 0 };
	snprintf(optionText, 100, "< %s >", m_sOptions[m_iCurOption]);
	pOptionText->SetText(_strdup(optionText));
	Post();
}

void EngineSelector::OnSelectLeftOption()
{
	m_iPrevOption = m_iCurOption;
	m_iCurOption--;
	m_iCurOption = (m_iCurOption + m_iNumOptions) % m_iNumOptions;

	char optionText[100] = { 0 };
	snprintf(optionText, 100, "< %s >", m_sOptions[m_iCurOption]);
	pOptionText->SetText(_strdup(optionText));
	pOptionText->SetText(m_sOptions[m_iCurOption]);
	Post();
}

void EngineSelector::Post()
{
	auto&& observers = selectorDispatcher.begin();

	// Trigger all other functions linked to this event type
	for (auto&& observer : selectorDispatcher)
	{
		observer();
	}
}

void EngineSelector::AddOption(const char* sOptionName)
{
	m_sOptions[m_iNumOptions] = sOptionName;

	if (m_iNumOptions == 0)
	{
		char optionText[100] = { 0 };
		snprintf(optionText, 100, "< %s >", sOptionName);
		pOptionText->SetText(_strdup(optionText));
	}

	m_iNumOptions++;
}

int EngineSelector::Destroy()
{
	m_pComponents->Remove(pOptionText);
	pOptionText->Destroy();
	SafeReleaseTemplate(pOptionText);

	int iRet = EnginePanel::Destroy();

	return iRet;
}
