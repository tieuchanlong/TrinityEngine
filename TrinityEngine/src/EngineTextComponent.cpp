#include <DirectX11BasePch.h>
#include "EngineTextComponent.h"
#include "Application.h"

#include "SimpleMath.h"
#include "EngineComponentManager.h"

EngineTextComponent::EngineTextComponent(const char* text, XMFLOAT3 vPos)
{
	m_sText = text;
	m_vLocalPos = vPos;
	m_bLocked = true;

	if (m_bLocked)
	{
		XMVECTOR vCameraChangedVector = Application::GetInstance()->GetMainCamera()->GetFocusPoint() - Application::GetInstance()->GetMainCamera()->GetOriginalFocusPoint();
		XMFLOAT3 vCamerChanged;
		XMStoreFloat3(&vCamerChanged, vCameraChangedVector);
		m_vLocalPos = m_vLocalPos + vCamerChanged;
	}

	EngineRenderComponent();
}

EngineTextComponent::EngineTextComponent() : EngineRenderComponent()
{
	m_bLocked = true;
}

void EngineTextComponent::RegisterGameClass()
{
	EngineComponentManager::GetInstance()->Register(ToStringHash("EngineRectangleComponent"), EngineTextComponent::Create);
}

bool EngineTextComponent::ReadSaveData(const rapidjson::Value& obj)
{
	return true;
}

bool EngineTextComponent::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	return true;
}

int EngineTextComponent::Initialize()
{
	int iRet = EngineRenderComponent::Initialize();

	m_fFont = new SpriteFont(Application::GetInstance()->GetDevice(), L"resources\\fonts\\myfile.spritefont");
	m_sSpriteBatch = new SpriteBatch(Application::GetInstance()->GetDeviceContext());

	return iRet;
}

int EngineTextComponent::PreUpdate()
{
	int iRet = EngineRenderComponent::PreUpdate();

	XMFLOAT4 vPointPos = XMFLOAT4(m_pOwner->GetWorldPosition().x + m_vLocalPos.x, m_pOwner->GetWorldPosition().y + m_vLocalPos.y, m_pOwner->GetWorldPosition().z + m_vLocalPos.z, 1.0f);
	XMVECTOR vPos = XMVector4Transform(XMLoadFloat4(&vPointPos), m_pWorldMatrix);
	m_vOriginalDistFromCameraFocus = vPos - Application::GetInstance()->GetMainCamera()->GetFocusPoint();

	return iRet;
}

int EngineTextComponent::Update(float fDeltaTime)
{
	int iRet = EngineRenderComponent::Update(fDeltaTime);

	// Revert the effect of camera global matrix to maintain the 2D matrix at same location
	if (m_bLocked && Application::GetInstance()->GetMainCamera()->IsCameraUpdated())
	{
		XMFLOAT4 vPointPos = XMFLOAT4(m_pOwner->GetWorldPosition().x + m_vLocalPos.x, m_pOwner->GetWorldPosition().y + m_vLocalPos.y, m_pOwner->GetWorldPosition().z + m_vLocalPos.z, 1.0f);
		XMVECTOR vPos = XMVector4Transform(XMLoadFloat4(&vPointPos), m_pWorldMatrix);
		XMVECTOR distVecToFocusPoint = vPos - Application::GetInstance()->GetMainCamera()->GetFocusPoint();
		//m_pWorldMatrix *= Application::GetInstance()->GetMainCamera()->GetLocalRotMatrix();

		// Translate the text component to supposed location
		XMVECTOR vChangeInDistVec = m_vOriginalDistFromCameraFocus - distVecToFocusPoint;
		XMFLOAT3 vChangeInDistVec3;
		XMStoreFloat3(&vChangeInDistVec3, vChangeInDistVec);

		vChangeInDistVec3.y = -vChangeInDistVec3.y;
		m_vLocalPos = m_vLocalPos + vChangeInDistVec3;

		Application::GetInstance()->ReRender();
	}

	return iRet;
}

int EngineTextComponent::Render()
{
	int iRet = Component::Render();

	m_sSpriteBatch->Begin();

	XMFLOAT3 vPixelPos = Application::GetInstance()->GetMainCamera()->GetPositionInCameraPixelViewSpace(m_pOwner->GetWorldPosition() + m_vLocalPos);

	XMVECTOR origin = m_fFont->MeasureString(m_sText) / 2.f;
	m_fFont->DrawString(m_sSpriteBatch, m_sText, XMLoadFloat3(&vPixelPos), Colors::White, 0.f, origin);

	m_sSpriteBatch->End();

	return iRet;
}

int EngineTextComponent::Destroy()
{
	int iRet = EngineRenderComponent::Destroy();

	SafeReleaseTemplate(m_fFont);
	SafeReleaseTemplate(m_sSpriteBatch);
	SafeReleaseTemplate(m_sText);

	return iRet;
}

int EngineTextComponent::BindVertexBuffer()
{
	return 0;
}

int EngineTextComponent::BindIndexBuffer()
{
	return 0;
}

int EngineTextComponent::BindConstantBuffer()
{
	return 0;
}

void EngineTextComponent::SetText(const char* sText)
{
	m_sText = sText;

	if (!GetOwner()->IsHidden())
	{
		Application::GetInstance()->ReRender();
	}
}
