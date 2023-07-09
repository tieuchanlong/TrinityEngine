#pragma once

#include "EngineBaseUI.h"
#include "EngineTextComponent.h"

class EngineText : public EngineBaseUI
{
public:
	EngineText(const char* text, XMFLOAT3 vPos);
	virtual int GetUIType() override { return UI_TEXT; }

	const char* GetText() { return m_pTextComp->GetText(); }
protected:
	EngineTextComponent* m_pTextComp;
};