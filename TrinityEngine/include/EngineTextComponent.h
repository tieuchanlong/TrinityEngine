#pragma once

#include <memory>
#include <SpriteFont.h>
#include <SpriteBatch.h>
#include "EngineRenderComponent.h"

using namespace DirectX;

class EngineTextComponent : public EngineRenderComponent
{
public:
	EngineTextComponent();
	EngineTextComponent(const char* text, XMFLOAT3 vPos);

	// --------------- Required for every component --------//
	static Component* Create() { return new EngineTextComponent(); }
	static void RegisterGameClass();
	virtual const char* GetComponentClassName() override { return "EngineTextComponent"; }
	virtual int GetComponentClassNameHash() override { return ToStringHash("EngineTextComponent"); }

	virtual bool ReadSaveData(const rapidjson::Value& obj);
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	// -------------------------------------------------//

	virtual int Initialize() override;
	virtual int PreUpdate() override;
	virtual int Update(float fDeltaTime) override;
	virtual int Render() override;
	virtual int Destroy() override;

	virtual int BindVertexBuffer() override;
	virtual int BindIndexBuffer() override;
	virtual int BindConstantBuffer() override;

	const char* GetText() { return m_sText; }
	virtual void SetText(const char* sText);

	virtual void SetLocked(bool bLocked) { m_bLocked = bLocked; }

protected:
	const char* m_sText;
	bool m_bLocked;
	XMVECTOR m_vOriginalDistFromCameraFocus;

	std::unique_ptr<SpriteFont> m_fFont;
	std::unique_ptr<SpriteBatch> m_sSpriteBatch;
};