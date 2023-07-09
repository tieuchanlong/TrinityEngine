#pragma once
#include "EnginePanel.h"
#include <functional>
#include <vector>

class EngineButton : public EnginePanel
{
public:
	EngineButton(XMFLOAT3 vPos, XMFLOAT3 vSize);
	EngineButton(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor);

	using ButtonFuncType = std::function<void()>;
	virtual void Subscribe(const std::function<void()>& newFunc);
	virtual void UnSubscribe(const ButtonFuncType& btnFunc);

	virtual int OnMousePressed(const MousePressedEvent& mousePressedEvent) override;
	virtual int OnMouseEnter(XMFLOAT3 vMousePos) override;
	virtual int OnMouseExit(XMFLOAT3 vMousePos) override;

	virtual int Destroy() override;
	virtual int Initialize() override;

	virtual int GetUIType() override { return UI_BUTTON; }

	virtual int Update(float fDeltaTime) override;

protected:
	static int m_iNumButtons;
	std::vector<ButtonFuncType> btnDispatcher;

	virtual void Post() override;
};