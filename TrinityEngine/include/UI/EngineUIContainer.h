#pragma once

#include "EngineBaseUI.h"
#include "SaveData/EngineUIData.h"

#define MAX_UI_LOAD_PER_FRAME 100

class EngineUIContainerData;

class EngineUIContainer : public Object
{
public:
	EngineUIContainer();

	// --------------- Required for every object --------//
	static Object* Create() { return new EngineUIContainer(); }
	static void RegisterGameClass();
	static const char* GetObjectClassName() { return "EngineUIContainer"; }
	static int GetObjectClassNameHash() { return ToStringHash("EngineUIContainer"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr);
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	// -------------------------------------------------//

	virtual void OnEnter();
	virtual void OnClick();
	virtual void OnExit();

	virtual void AddUI(EngineBaseUI* newUI) { m_pBaseUIs->Add(newUI); }

	virtual int Initialize() override;
	virtual int PreUpdate() override;
	virtual int Update(float fDeltaTime) override;
	virtual int Render() override;
	virtual int Destroy() override;

	// UI trigger events for mouse
	virtual int OnMousePressed(const MousePressedEvent& mousePressedEvent);
	virtual int OnMouseEnter(XMFLOAT3 vMousePos);
	virtual int OnMouseExit(XMFLOAT3 vMousePos);

	// UI trigger events for keyboard
	virtual int OnSelectPrevOption();
	virtual int OnSelectNextOption();
	virtual int OnConfirmOption();
	virtual int OnSelectRightOption();
	virtual int OnSelectLeftOption();

	virtual EngineBaseUI* GetUIByIndex(int iIndex);
	virtual EngineBaseUI* GetUIByName(const char* sUIName);
	virtual int GetNumUI() { return m_pBaseUIs->GetNumElements(); }

	virtual bool IsInUISpace(XMFLOAT3 vMousePos);

	virtual int GetMenuType() { return m_iMenuType; }
	virtual void SetMenuType(int iMenuType) { m_iMenuType = iMenuType; }

	virtual void SetNextUIContainer(int iNextUIContainer) { m_iNextUIContainer = iNextUIContainer; }
	virtual int GetNextUIContainer() { return m_iNextUIContainer; }

protected:
	List<EngineBaseUI>* m_pBaseUIs;
	List<EngineBaseUI>* m_pInteractibleUIs;
	const char* m_sUIPath;

	EngineBaseUI* m_pMouseEnteredBaseUI;
	int m_iHighlightedUIIndex;

	int m_iMenuType;

	int m_iNextUIContainer;

private: 
	void TestUIContainer();
};