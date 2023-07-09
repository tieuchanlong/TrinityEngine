#pragma once
#include "UI/EnginePanel.h"
#include "EngineTextComponent.h"

class EngineSlider : public EnginePanel
{
public:
	EngineSlider(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vTextPos, float fValue, float fIncreaseStep, float fMinValue, float fMaxValue, XMFLOAT3 vColor);

	using SelectorFuncType = std::function<void()>;
	virtual void Subscribe(const SelectorFuncType& newFunc);
	virtual void UnSubscribe(const SelectorFuncType& btnFunc);

	virtual int GetUIType() override { return UI_SLIDER; }

	virtual void OnSelectRightOption();
	virtual void OnSelectLeftOption();

	virtual void Post() override;

	virtual int Destroy() override;

	virtual float GetValue() { return m_fValue; }
	virtual void SetValue(float fValue);
	virtual void SetMinValue(float fMinValue) { m_fMinValue = fMinValue; }
	virtual void SetMaxValue(float fMaxValue) { m_fMaxValue = fMaxValue; }
protected:
	std::vector<SelectorFuncType> sliderDispatcher;

	float m_fValue;
	float m_fIncreaseStep;
	float m_fMinValue;
	float m_fMaxValue;

	EngineTextComponent* m_pValueText;
};