#include <DirectX11BasePch.h>
#include "UI/EngineGUIManager.h"
#include "Event/EngineEventManager.h"
#include "UI/EnginePanel.h"
#include "UI/EngineButton.h"
#include "UI/EngineText.h"
#include "UI/EngineSelector.h"
#include "EngineMouse.h"
#include "EngineObjectManager.h"
#include "EngineAccessor.h"

const char* g_sUIPath[NUM_MENU_TYPES] = {
	"\\common\\UI\\MainUI.json",
	"\\common\\UI\\TitleMenu.json",
	"\\common\\UI\\TransitionMenu.json",
	"\\common\\UI\\ObjectEditorMenu.json",
	"\\common\\UI\\CreatureEditorMenu.json",
	"\\common\\UI\\MapEditorMenu.json",
	"\\common\\UI\\LevelEditorMenu.json",
	"\\common\\UI\\PauseMenu.json",
};

EngineGUIManager* EngineGUIManager::instance = nullptr;
EngineGUIManager* EngineGUIManager::GetInstance()
{
	if (!instance)
	{
		instance = new EngineGUIManager();
	}

	return instance;
}

int EngineGUIManager::Initialize()
{
	EngineEventManager::GetInstance()->SubscribeFunction(EventType::MouseButtonPressed, std::bind(&EngineGUIManager::OnMousePressed, this, std::placeholders::_1));
	EngineEventManager::GetInstance()->SubscribeFunction(EventType::SelectPrevOption, std::bind(&EngineGUIManager::OnSelectPrevOption, this, std::placeholders::_1));
	EngineEventManager::GetInstance()->SubscribeFunction(EventType::SelectNextOption, std::bind(&EngineGUIManager::OnSelectNextOption, this, std::placeholders::_1));
	EngineEventManager::GetInstance()->SubscribeFunction(EventType::ConfirmOption, std::bind(&EngineGUIManager::OnConfirmOption, this, std::placeholders::_1));
	EngineEventManager::GetInstance()->SubscribeFunction(EventType::SelectLeftOption, std::bind(&EngineGUIManager::OnSelectLeftOption, this, std::placeholders::_1));
	EngineEventManager::GetInstance()->SubscribeFunction(EventType::SelectRightOption, std::bind(&EngineGUIManager::OnSelectRightOption, this, std::placeholders::_1));
	EngineEventManager::GetInstance()->SubscribeFunction(EventType::ChangeMenu, std::bind(&EngineGUIManager::OnChangeMenu, this, std::placeholders::_1));
	
	AddInitialUI();

	// Initialize UI
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Initialize();
	}

	return 0;
}

int EngineGUIManager::PreUpdate()
{
	// Remove unnecessary UI first
	if (m_pRemoveUIStack->GetNumElements() > 0)
	{
		for (Iterator<EngineUIContainer>* it = m_pRemoveUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			it->GetValue()->Destroy();
		}
		m_pRemoveUIStack->Clear();
		Application::GetInstance()->ReRender();
	}

	// Add new UI
	if (m_pAddUIStack->GetNumElements() > 0)
	{
		for (Iterator<EngineUIContainer>* it = m_pAddUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			if (!it->GetValue()->IsInitialized())
			{
				it->GetValue()->Initialize();

				if (it->GetValue()->GetMenuType() != PAUSE_MENU)
				{
					m_pUIStack->Add(it->GetValue());
				}
				else
				{
					m_pUIStack->AddFront(it->GetValue());
				}
			}
		}
		m_pAddUIStack->Clear();
		Application::GetInstance()->ReRender();
	}

	// PreUpdate UI
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->PreUpdate();
	}

	return 0;
}

int EngineGUIManager::Update(float fDeltaTime)
{
	// Update UI
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Update(fDeltaTime);
	}

	if (m_pUIStack->GetNumElements() > 0 && m_bTransitionMenuActive)
	{
		// Hide other UIs when the transition menu is active
		for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			if (!it->GetValue()->IsActive() || it->GetValue()->GetMenuType() == TRANSITION_MENU)
			{
				continue;
			}

			it->GetValue()->SetHidden(true);
		}
	}

	if (EngineEventManager::GetInstance()->GetCurInputType() == InputType::MOUSE_INPUT && EngineMouse::GetInstance()->IsActive())
	{
		OnMouseEntered();
	}

	return 0;
}

int EngineGUIManager::Render()
{
	// Update UI
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Render();
	}

	return 0;
}

int EngineGUIManager::Destroy()
{
	// Destroy UIs
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		it->GetValue()->Destroy();
	}

	// Clean UIs
	m_pUIStack->Clear();
	m_pRemoveUIStack->Clear();
	m_pAddUIStack->Clear();
	SafeReleaseTemplate(m_pUIStack);
	SafeReleaseTemplate(m_pRemoveUIStack);
	SafeReleaseTemplate(m_pAddUIStack);

	return 0;
}

void EngineGUIManager::ClearAllMenus()
{
	if (m_pUIStack->GetNumElements() > 0)
	{
		for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			m_pRemoveUIStack->Add(it->GetValue());
		}

		m_pUIStack->Clear();
	}
}

void EngineGUIManager::AddMenu(int iUIType)
{
	if (iUIType == PAUSE_MENU)
	{
		// Hide other menus when pause menu is active
		for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			it->GetValue()->SetHidden(true);
		}
	}

	char fullUIPath[256] = { 0 };
	snprintf(fullUIPath, 256, "%s%s", Application::GetInstance()->GetCurDirectory(), g_sUIPath[iUIType]);
	EngineUIContainer* pMenu = dynamic_cast<EngineUIContainer*>(GET_OBJECT_MANAGER()->CreateObjectBySource(fullUIPath));
	pMenu->SetMenuType(iUIType);
	m_pAddUIStack->Add(pMenu);

	if (iUIType == TRANSITION_MENU)
	{
		m_bTransitionMenuActive = true;
	}
}

void EngineGUIManager::RemoveMenu(int iUIType)
{
	// Check Mouse Pressed event for each UI
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->GetMenuType() == iUIType)
		{
			m_pRemoveUIStack->Add(it->GetValue());
			m_pUIStack->Remove(it->GetValue());
			break;
		}
	}

	if (iUIType == TRANSITION_MENU)
	{
		m_bTransitionMenuActive = false;

		// Show other UIs again when the transition menu is off
		for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			if (!it->GetValue()->IsActive())
			{
				continue;
			}

			it->GetValue()->SetHidden(false);
		}
	}

	if (iUIType == PAUSE_MENU)
	{
		// Hide other menus when pause menu is active
		for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			it->GetValue()->SetHidden(false);
		}
	}
}

EngineUIContainer* EngineGUIManager::GetUIByType(int iUIType)
{
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->GetMenuType() == iUIType)
		{
			return it->GetValue();
		}
	}

	return 0;
}

void EngineGUIManager::OnMouseEntered()
{
	XMFLOAT3 vMousePos = XMFLOAT3(EngineMouse::GetInstance()->GetState().x, EngineMouse::GetInstance()->GetState().y, 0);

	// Check for mouse enter event
	if (!m_pMouseEnteredUI)
	{
		for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
		{
			if (!it->GetValue()->IsActive())
			{
				continue;
			}

			if (it->GetValue()->IsInUISpace(vMousePos))
			{
				m_pMouseEnteredUI = it->GetValue();
				m_pMouseEnteredUI->OnMouseEnter(vMousePos);
				break;
			}
		}
	}
	else
	{
		if (!m_pMouseEnteredUI->IsInUISpace(vMousePos))
		{
			m_pMouseEnteredUI->OnMouseExit(vMousePos);
			m_pMouseEnteredUI = 0;
		}
	}
}

void EngineGUIManager::OnMousePressed(const Event& e)
{
	int iRet = 0;
	const MousePressedEvent mousePressedEvent = static_cast<const MousePressedEvent&>(e);

	// Check Mouse Pressed event for each UI
	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		iRet = it->GetValue()->OnMousePressed(mousePressedEvent);

		if (iRet >= 0)
		{
			break;
		}
	}
}

void EngineGUIManager::OnSelectPrevOption(const Event& e)
{
	if (m_pMouseEnteredUI)
	{
		m_pMouseEnteredUI->OnMouseExit(XMFLOAT3(-10000, -10000, -10000));
		m_pMouseEnteredUI = 0;
	}

	if (m_pUIStack->GetHead())
	{
		m_pUIStack->GetHead()->GetValue()->OnSelectPrevOption();
	}
}

void EngineGUIManager::OnSelectNextOption(const Event& e)
{
	if (m_pMouseEnteredUI)
	{
		m_pMouseEnteredUI->OnMouseExit(XMFLOAT3(-10000, -10000, -10000));
		m_pMouseEnteredUI = 0;
	}

	if (m_pUIStack->GetHead())
	{
		m_pUIStack->GetHead()->GetValue()->OnSelectNextOption();
	}
}

void EngineGUIManager::OnSelectRightOption(const Event& e)
{
	if (m_pUIStack->GetHead())
	{
		m_pUIStack->GetHead()->GetValue()->OnSelectRightOption();
	}
}

void EngineGUIManager::OnSelectLeftOption(const Event& e)
{
	if (m_pUIStack->GetHead())
	{
		m_pUIStack->GetHead()->GetValue()->OnSelectLeftOption();
	}
}

void EngineGUIManager::OnConfirmOption(const Event& e)
{
	if (m_pUIStack->GetHead())
	{
		m_pUIStack->GetHead()->GetValue()->OnConfirmOption();
	}
}

void EngineGUIManager::OnChangeMenu(const Event& e)
{
	int iNextUIContainer = m_pUIStack->GetHead()->GetValue()->GetNextUIContainer();
	EngineUIContainer* pNextUIContainer = 0;

	for (Iterator<EngineUIContainer>* it = m_pUIStack->GetHead(); it != NULL; it = it->GetNext())
	{
		if (it->GetValue()->GetMenuType() == iNextUIContainer)
		{
			pNextUIContainer = it->GetValue();
			break;
		}
	}

	if (!pNextUIContainer)
	{
		return;
	}

	m_pUIStack->Remove(pNextUIContainer);
	m_pUIStack->AddFront(pNextUIContainer);
}

EngineGUIManager::EngineGUIManager()
{
	m_pUIStack = new List<EngineUIContainer>();
	m_pRemoveUIStack = new List<EngineUIContainer>();
	m_pAddUIStack = new List<EngineUIContainer>();
	m_pMouseEnteredUI = 0;
	m_bTransitionMenuActive = false;
}

void EngineGUIManager::AddInitialUI()
{
	AddMenu(TITLE_MENU);
}
