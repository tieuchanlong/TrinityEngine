#pragma once
#include "Application.h"
#include "UI/EngineGUIManager.h"
#include "EngineObjectManager.h"

#define GET_LEVEL_MANAGER() Application::GetInstance()->GetLevelManager()
#define GET_GUI_MANAGER() EngineGUIManager::GetInstance()
#define GET_OBJECT_MANAGER() EngineObjectManager::GetInstance()
#define GET_CURRENT_LEVEL() Application::GetInstance()->GetLevelManager()->GetCurrentLevel()