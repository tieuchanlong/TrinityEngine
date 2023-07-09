#include <DirectX11BasePch.h>
#include "EngineCubeComponent.h"
#include "EngineComponentManager.h"
#include "EngineObject.h"


EngineCubeComponent::EngineCubeComponent(XMFLOAT3 vPos, XMFLOAT3 vSize) : EngineRenderComponent()
{
	m_vLocalPos = vPos;
	m_vSize = vSize;
	m_vColor = XMFLOAT3(0, 1, 0);
}

EngineCubeComponent::EngineCubeComponent(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor)
{
	m_vLocalPos = vPos;
	m_vSize = vSize;
	m_vColor = vColor;
}

EngineCubeComponent::EngineCubeComponent() : EngineRenderComponent()
{

}

int EngineCubeComponent::Initialize()
{
	SetupVerticesAndIndices();

	int iRet = EngineRenderComponent::Initialize();

	return iRet;
}

void EngineCubeComponent::RegisterGameClass()
{
	EngineComponentManager::GetInstance()->Register(ToStringHash("EngineCubeComponent"), EngineCubeComponent::Create);
}

void EngineCubeComponent::SetupVerticesAndIndices()
{
	if (m_bUpdateVertexBuffer)
	{
		m_lVertices->Clear();
	}

	XMFLOAT3 vPos = m_pOwner->GetWorldPosition() + m_vLocalPos;

	// Add sample vertices for a cube
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x - m_vSize.x / 2, vPos.y - m_vSize.y / 2, vPos.z - m_vSize.z / 2), m_vColor })); // 0
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x - m_vSize.x / 2, vPos.y + m_vSize.y / 2, vPos.z - m_vSize.z / 2), m_vColor })); // 1
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x + m_vSize.x / 2, vPos.y + m_vSize.y / 2, vPos.z - m_vSize.z / 2), m_vColor })); // 2
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x + m_vSize.x / 2, vPos.y - m_vSize.y / 2, vPos.z - m_vSize.z / 2), m_vColor })); // 3
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x - m_vSize.x / 2, vPos.y - m_vSize.y / 2, vPos.z + m_vSize.z / 2), m_vColor })); // 4
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x - m_vSize.x / 2, vPos.y + m_vSize.y / 2, vPos.z + m_vSize.z / 2), m_vColor })); // 5
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x + m_vSize.x / 2, vPos.y + m_vSize.y / 2, vPos.z + m_vSize.z / 2), m_vColor })); // 6
	m_lVertices->Add(new VertexPosColor({ XMFLOAT3(vPos.x + m_vSize.x / 2, vPos.y - m_vSize.y / 2, vPos.z + m_vSize.z / 2), m_vColor })); // 7

	if (!m_bUpdateVertexBuffer)
	{
		// Add sample indices order
		m_lIndices->Add(new int(0)); m_lIndices->Add(new int(1)); m_lIndices->Add(new int(2)); m_lIndices->Add(new int(0)); m_lIndices->Add(new int(2)); m_lIndices->Add(new int(3));
		m_lIndices->Add(new int(4)); m_lIndices->Add(new int(6)); m_lIndices->Add(new int(5)); m_lIndices->Add(new int(4)); m_lIndices->Add(new int(7)); m_lIndices->Add(new int(6));
		m_lIndices->Add(new int(4)); m_lIndices->Add(new int(5)); m_lIndices->Add(new int(1)); m_lIndices->Add(new int(4)); m_lIndices->Add(new int(1)); m_lIndices->Add(new int(0));
		m_lIndices->Add(new int(3)); m_lIndices->Add(new int(2)); m_lIndices->Add(new int(6)); m_lIndices->Add(new int(3)); m_lIndices->Add(new int(6)); m_lIndices->Add(new int(7));
		m_lIndices->Add(new int(1)); m_lIndices->Add(new int(5)); m_lIndices->Add(new int(6)); m_lIndices->Add(new int(1)); m_lIndices->Add(new int(6)); m_lIndices->Add(new int(2));
		m_lIndices->Add(new int(4)); m_lIndices->Add(new int(0)); m_lIndices->Add(new int(3)); m_lIndices->Add(new int(4)); m_lIndices->Add(new int(3)); m_lIndices->Add(new int(7));
	}
}

