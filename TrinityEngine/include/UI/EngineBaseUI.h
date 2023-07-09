#pragma once

#include "EngineObject.h"
#include "EngineList.h"
#include "Event/MousePressedEvent.h"

enum UIType
{
	UI_PANEL,
	UI_BUTTON,
	UI_TEXT,
	UI_SELECTOR,
	UI_SLIDER,
	NUM_UI_TYPES,
};

class EngineBaseUI : public Object
{
public:
	EngineBaseUI(XMFLOAT3 vPos, XMFLOAT3 vSize);

	virtual void OnEnter();
	virtual void OnClick();
	virtual void OnExit();

	virtual bool IsInUISpace(const XMFLOAT3 vPos) { return false; }

	virtual int OnMousePressed(const MousePressedEvent& mousePressedEvent) { return 0; }
	virtual int OnMouseEnter(XMFLOAT3 vMousePos);
	virtual int OnMouseExit(XMFLOAT3 vMousePos);

	virtual int OnHighlighted() { return 0; }
	virtual int OnUnHighlighted() { return 0; }

	virtual bool IsMouseEntered() { return m_bMouseEntered; }
	virtual bool IsMouseExit() { return m_bMouseExit; }

	virtual int GetUIType() = 0;

	virtual void Post() {}

protected:
	bool m_bMouseEntered;
	bool m_bMouseExit;
};