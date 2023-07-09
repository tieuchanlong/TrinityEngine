#pragma once
#include "EngineRectangleComponent.h"
#include "EngineBaseUI.h"

class EnginePanel : public EngineBaseUI
{
public:
	EnginePanel(XMFLOAT3 vPos, XMFLOAT3 vSize);
	EnginePanel(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor);

	virtual bool IsInUISpace(const XMFLOAT3 vPos) override;

	virtual int OnMousePressed(const MousePressedEvent& mousePressedEvent) override;
	virtual int OnMouseEnter(XMFLOAT3 vMousePos) override;
	virtual int OnMouseExit(XMFLOAT3 vMousePos) override;

	virtual int OnHighlighted() override;
	virtual int OnUnHighlighted() override;

	virtual int GetUIType() override { return UI_PANEL; }

	virtual void SetHighlightColor(XMFLOAT3 vHighlightColor) { m_vHighlightColor = vHighlightColor; }

protected:
	static int m_iNumPanels;
	EngineRectangleComponent* m_pRectComp;

	XMFLOAT3 m_vHighlightColor;
	XMFLOAT3 m_vMainColor;
};