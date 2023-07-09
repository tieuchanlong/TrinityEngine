#pragma once
#include "EngineUIContainer.h"
#include "Event/MousePressedEvent.h"

enum MenuType
{
	MAIN_UI_MENU = 0,
	TITLE_MENU,
	TRANSITION_MENU,
	OBJECT_EDITOR_MENU,
	CREATURE_EDITOR_MENU,
	MAP_EDITOR_MENU,
	LEVEL_EDITOR_MENU,
	PAUSE_MENU,
	NUM_MENU_TYPES
};

class EngineGUIManager
{
public:
	static EngineGUIManager* GetInstance();

	virtual int Initialize();
	virtual int PreUpdate();
	virtual int Update(float fDeltaTime);
	virtual int Render();
	virtual int Destroy();

	virtual void ClearAllMenus();
	virtual void AddMenu(int iUIType);
	virtual void RemoveMenu(int iUIType);

	virtual EngineUIContainer* GetUIByType(int iUIType);

	virtual void OnMouseEntered();
	virtual void OnMousePressed(const Event& e);
	virtual void OnSelectPrevOption(const Event& e);
	virtual void OnSelectNextOption(const Event& e);
	virtual void OnSelectRightOption(const Event& e);
	virtual void OnSelectLeftOption(const Event& e);
	virtual void OnConfirmOption(const Event& e);
	virtual void OnChangeMenu(const Event& e);

	virtual void AddInitialUI();

protected:
	static EngineGUIManager* instance;
	EngineGUIManager();

	bool m_bTransitionMenuActive;

	// UIs management
	List<EngineUIContainer>* m_pUIStack;
	List<EngineUIContainer>* m_pRemoveUIStack;
	List<EngineUIContainer>* m_pAddUIStack;
	EngineUIContainer* m_pMouseEnteredUI;
};