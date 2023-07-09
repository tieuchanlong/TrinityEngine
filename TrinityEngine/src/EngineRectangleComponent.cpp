#include <DirectX11BasePch.h>
#include "EngineRectangleComponent.h"
#include "Application.h"
#include "EngineComponentManager.h"

EngineRectangleComponent::EngineRectangleComponent(XMFLOAT3 vPos, XMFLOAT3 vSize)
{
	m_vLocalPos = vPos;
	m_vSize = vSize;
	m_vColor = XMFLOAT3(1, 0, 0);
}

EngineRectangleComponent::EngineRectangleComponent(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor)
{
	m_vLocalPos = vPos;
	m_vSize = vSize;
	m_vColor = vColor;
}

EngineRectangleComponent::EngineRectangleComponent() : EngineRenderComponent()
{

}

void EngineRectangleComponent::RegisterGameClass()
{
	EngineComponentManager::GetInstance()->Register(ToStringHash("EngineRectangleComponent"), EngineRectangleComponent::Create);
}

int EngineRectangleComponent::Initialize()
{
	SetupVerticesAndIndices();

	int iRet = EngineRenderComponent::Initialize();
	return iRet;
}

int EngineRectangleComponent::PreUpdate()
{
	int iRet = EngineRenderComponent::PreUpdate();

	XMFLOAT4 vPointPos = XMFLOAT4(m_pOwner->GetWorldPosition().x + m_vLocalPos.x, m_pOwner->GetWorldPosition().y + m_vLocalPos.y, m_pOwner->GetWorldPosition().z + m_vLocalPos.z, 1.0f);
	XMVECTOR vPos = XMVector4Transform(XMLoadFloat4(&vPointPos), m_pWorldMatrix);
	m_vOriginalDistFromCameraFocus = vPos - Application::GetInstance()->GetMainCamera()->GetFocusPoint();

	return iRet;
}

int EngineRectangleComponent::Render()
{
	int iRet = Component::Render();

	// Create the view matrix 
	m_pViewMatrix = XMMatrixLookAtLH(Application::GetInstance()->GetMainCamera()->GetEyePosition(), Application::GetInstance()->GetMainCamera()->GetFocusPoint(), Application::GetInstance()->GetMainCamera()->GetUpDirection());

	Application::GetInstance()->GetDeviceContext()->UpdateSubresource(m_pd3dConstantBuffers[CB_Frame], 0, nullptr, &m_pViewMatrix, 0, 0); // The constant buffer per frame will now store the value of g_ViewMatrix

	Application::GetInstance()->GetDeviceContext()->UpdateSubresource(m_pd3dConstantBuffers[CB_Object], 0, nullptr, &m_pWorldMatrix, 0, 0);

	// Prepare input assembler for the rendering pipeline
	const UINT vertexStride = sizeof(VertexPosColor);
	const UINT offset = 0;

	// Set vertex and index buffer
	Application::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &vertexStride, &offset); // Bind the Index Buffer to Input Assembler stage
	Application::GetInstance()->GetDeviceContext()->IASetInputLayout(m_pd3dInputLayout);
	Application::GetInstance()->GetDeviceContext()->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);  // Bind the Index Buffer
	Application::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // Set what primitive shapes we are using to render, and currently we are using triangles

	// Prepare the vertex shader stage. We only need to specify which vertex shader will be used
	Application::GetInstance()->GetDeviceContext()->VSSetShader(m_pd3dVertexShader, nullptr, 0); // Vertex shader is bound to this stage using this method
	Application::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(0, 3, m_pd3dConstantBuffers);

	// Set up the rasterizer stage
	Application::GetInstance()->GetDeviceContext()->RSSetState(Application::GetInstance()->GetRasterizerState());
	Application::GetInstance()->GetDeviceContext()->RSSetViewports(1, Application::GetInstance()->GetViewPort()); // Bound the viewport to this stage

	// Set the pixel shader to the rasterizer stage
	Application::GetInstance()->GetDeviceContext()->PSSetShader(m_pd3dPixelShader, nullptr, 0);

	// Set up the output merger stage. Merges the output from pixel shader to color and depth buffer
	Application::GetInstance()->GetDeviceContext()->OMSetRenderTargets(1, Application::GetInstance()->GetRenderTargetView(), Application::GetInstance()->GetDepthStencilView()); // Bound back buffer and depth buffer to this stage
	Application::GetInstance()->GetDeviceContext()->OMSetDepthStencilState(Application::GetInstance()->GetDepthStencilState(), 1);

	// Draw the shape using indices
	Application::GetInstance()->GetDeviceContext()->DrawIndexed(m_lIndices->GetNumElements(), 0, 0);

	return iRet;
}

void EngineRectangleComponent::SetupVerticesAndIndices()
{
	XMFLOAT3 vPos = m_pOwner->GetWorldPosition() + m_vLocalPos;

	if (m_bUpdateVertexBuffer)
	{
		m_lVertices->Clear();
	}

	// Add sample vertices for a cube
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x - m_vSize.x / 2, vPos.y - m_vSize.y / 2, vPos.z), m_vColor })); // 0
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x + m_vSize.x / 2, vPos.y - m_vSize.y / 2, vPos.z), m_vColor })); // 1
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x - m_vSize.x / 2, vPos.y + m_vSize.y / 2, vPos.z), m_vColor })); // 2
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x + m_vSize.x / 2, vPos.y + m_vSize.y / 2, vPos.z), m_vColor })); // 3

	if (!m_bUpdateVertexBuffer)
	{
		// Add sample indices order
		m_lIndices->Add(new int(0)); m_lIndices->Add(new int(1)); m_lIndices->Add(new int(3)); m_lIndices->Add(new int(0));
		m_lIndices->Add(new int(3)); m_lIndices->Add(new int(2)); m_lIndices->Add(new int(0)); m_lIndices->Add(new int(3));
	}
}

void EngineRectangleComponent::PrepareViewAndRotationMatrix(float fDeltaTime)
{
	EngineRenderComponent::PrepareViewAndRotationMatrix(fDeltaTime);

	XMFLOAT3 vPos = m_pOwner->GetWorldPosition() + m_vLocalPos;

	// Revert the effect of camera global matrix to maintain the 2D matrix at same location
	if (Application::GetInstance()->GetMainCamera()->IsCameraUpdated())
	{
		XMFLOAT4 vPointPos = XMFLOAT4(vPos.x, vPos.y, vPos.z, 1.0f);
		XMVECTOR vPos = XMVector4Transform(XMLoadFloat4(&vPointPos), m_pWorldMatrix);
		XMVECTOR distVecToFocusPoint = vPos - Application::GetInstance()->GetMainCamera()->GetFocusPoint();
		//m_pWorldMatrix *= Application::GetInstance()->GetMainCamera()->GetLocalRotMatrix();

		// Translate the rectangle component to supposed location
		XMVECTOR vChangeInDistVec = m_vOriginalDistFromCameraFocus - distVecToFocusPoint;
		XMFLOAT3 vChangeInDistVec3;
		XMStoreFloat3(&vChangeInDistVec3, vChangeInDistVec);

		XMMATRIX translationMatrix = XMMatrixTranslation(vChangeInDistVec3.x, vChangeInDistVec3.y, vChangeInDistVec3.z);
		m_pWorldMatrix *= translationMatrix;

		Application::GetInstance()->ReRender();
	}
}
