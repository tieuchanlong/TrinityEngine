#include <DirectX11BasePch.h>
#include "UI/EngineSlider.h"
#include "Application.h"

EngineSlider::EngineSlider(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vTextPos, float fValue, float fIncreaseStep, float fMinValue, float fMaxValue, XMFLOAT3 vColor) : EnginePanel(vPos, vSize, vColor)
{
	m_fValue = fValue;
	m_fMinValue = fMinValue;
	m_fMaxValue = fMaxValue;
	m_fIncreaseStep = fIncreaseStep;

	char sValue[50] = { 0 };
	snprintf(sValue, 50, "<%.2f>", m_fValue);
	m_pValueText = new EngineTextComponent(_strdup(sValue), vPos + vTextPos);
	AddComponent(m_pValueText);
}

void EngineSlider::Subscribe(const SelectorFuncType& newFunc)
{
	sliderDispatcher.push_back(newFunc);
}

void EngineSlider::UnSubscribe(const SelectorFuncType& btnFunc)
{

}

void EngineSlider::OnSelectRightOption()
{
	m_fValue += m_fIncreaseStep;

	if (m_fValue > m_fMaxValue)
	{
		m_fValue = m_fMaxValue;
	}

	char sValue[50] = { 0 };
	snprintf(sValue, 50, "<%.2f>", m_fValue);
	m_pValueText->SetText(_strdup(sValue));

	Post();

	if (!IsHidden())
	{
		Application::GetInstance()->ReRender();
	}
}

void EngineSlider::OnSelectLeftOption()
{
	m_fValue -= m_fIncreaseStep;

	if (m_fValue < m_fMinValue)
	{
		m_fValue = m_fMinValue;
	}

	char sValue[50] = { 0 };
	snprintf(sValue, 50, "<%.2f>", m_fValue);
	m_pValueText->SetText(_strdup(sValue));

	Post();

	if (!IsHidden())
	{
		Application::GetInstance()->ReRender();
	}
}

void EngineSlider::Post()
{
	auto&& observers = sliderDispatcher.begin();

	// Trigger all other functions linked to this event type
	for (auto&& observer : sliderDispatcher)
	{
		observer();
	}
}

int EngineSlider::Destroy()
{
	m_pComponents->Remove(m_pValueText);
	int iRet = EnginePanel::Destroy();

	m_pValueText->Destroy();
	SafeReleaseTemplate(m_pValueText);

	return iRet;
}

void EngineSlider::SetValue(float fValue)
{
	if (m_fValue != fValue)
	{
		char sValue[50] = { 0 };
		snprintf(sValue, 50, "<%.2f>", fValue);
		m_pValueText->SetText(_strdup(sValue));

		Post();

		if (!IsHidden())
		{
			Application::GetInstance()->ReRender();
		}
	}

	m_fValue = fValue;
}
