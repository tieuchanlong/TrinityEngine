#pragma once

#include "UI/EnginePanel.h"
#include "EngineTextComponent.h"

#define MAX_SELECTOR_OPTIONS 100

class EngineSelector : public EnginePanel
{
public:
	EngineSelector(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vTextPos);
	EngineSelector(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vTextPos, XMFLOAT3 vColor);

	using SelectorFuncType = std::function<void()>;
	virtual void Subscribe(const SelectorFuncType& newFunc);
	virtual void UnSubscribe(const SelectorFuncType& btnFunc);

	virtual int GetUIType() override { return UI_SELECTOR; }

	virtual void OnSelectRightOption();
	virtual void OnSelectLeftOption();

	virtual void Post() override;

	virtual void AddOption(const char* sOptionName);
	int GetSelectedOption() { return m_iCurOption; }
	int GetPrevOption() { return m_iPrevOption; }

	virtual int Destroy() override;
protected:
	int m_iNumOptions;
	int m_iCurOption;
	int m_iPrevOption;
	std::vector<SelectorFuncType> selectorDispatcher;

	EngineTextComponent* pOptionText;

	const char* m_sOptions[MAX_SELECTOR_OPTIONS];
};