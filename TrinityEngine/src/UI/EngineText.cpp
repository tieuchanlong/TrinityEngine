#include <DirectX11BasePch.h>
#include "UI/EngineText.h"

EngineText::EngineText(const char* text, XMFLOAT3 vPos) : EngineBaseUI(vPos, XMFLOAT3(0, 0, 0))
{
	m_pTextComp = new EngineTextComponent(text, vPos);
	m_pTextComp->SetLocked(true);
	AddComponent(m_pTextComp);
}
