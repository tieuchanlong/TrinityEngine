#include <DirectX11BasePch.h>
#include "EngineRenderComponent.h"
#include "Application.h"
#include "EngineCamera.h"
#include "Physics/EngineRigidBodyComponent.h"
#include "EngineComponentManager.h"


EngineRenderComponent::EngineRenderComponent() : Component()
{
	m_lVertices = new List<VertexPosColor>();
	m_lIndices = new List<int>();
	m_pWorldMatrix = XMMatrixIdentity();
	m_bUpdateVertexBuffer = false;
	m_fAlpha = 1.0f;
}

void EngineRenderComponent::RegisterGameClass()
{
	EngineComponentManager::GetInstance()->Register(ToStringHash("EngineRenderComponent"), EngineRenderComponent::Create);
}

bool EngineRenderComponent::ReadSaveData(rapidjson::Value::ConstValueIterator& itr)
{
	bool bRet = Component::ReadSaveData(itr);

	const rapidjson::Value& obj = (*itr);

	m_vLocalPos = XMFLOAT3(obj["X"].GetDouble(), obj["Y"].GetDouble(), obj["Z"].GetDouble());
	m_vSize = XMFLOAT3(obj["SizeX"].GetDouble(), obj["SizeY"].GetDouble(), obj["SizeZ"].GetDouble());
	m_vColor = XMFLOAT3(obj["ColorR"].GetDouble(), obj["ColorG"].GetDouble(), obj["ColorB"].GetDouble());
	itr++;

	return bRet;
}

bool EngineRenderComponent::WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer)
{
	bool bRet = Component::WriteSaveData(writer);

	writer->StartObject();
	writer->Key("X");
	writer->Double(m_vLocalPos.x);
	writer->Key("Y");
	writer->Double(m_vLocalPos.y);
	writer->Key("Z");
	writer->Double(m_vLocalPos.z);
	writer->Key("SizeX");
	writer->Double(m_vSize.x);
	writer->Key("SizeY");
	writer->Double(m_vSize.y);
	writer->Key("SizeZ");
	writer->Double(m_vSize.z);
	writer->Key("ColorR");
	writer->Double(m_vColor.x);
	writer->Key("ColorG");
	writer->Double(m_vColor.y);
	writer->Key("ColorB");
	writer->Double(m_vColor.z);
	writer->EndObject();

	return true;
}

int EngineRenderComponent::Initialize()
{
	int iRet = Component::Initialize();

	// Start binding vertex and index buffer and constant buffer
	iRet |= BindVertexBuffer();
	iRet |= BindIndexBuffer();
	iRet |= BindConstantBuffer();
	
	// Start binding shaders
	iRet != BindVertexShader();
	iRet |= BindPixelShader();

	// Prep the initial values for position and rotational positions
	m_fAngle = 0.0f;
	PrepareViewAndRotationMatrix(0);

	SetupBlendState();

	return iRet;
}

int EngineRenderComponent::Update(float fDeltaTime)
{
	int iRet = Component::Update(fDeltaTime);

	// Update vertex buffer if triggered
	if (m_bUpdateVertexBuffer)
	{
		SetupVerticesAndIndices();

		// Convert the list of vertices into an array
		int iNumVertices = m_lVertices->GetNumElements();
		int iCurVertex = 0;
		VertexPosColor* pVertices = new VertexPosColor[iNumVertices];

		for (Iterator<VertexPosColor>* it = m_lVertices->GetHead(); it != NULL; it = it->GetNext())
		{
			pVertices[iCurVertex] = *it->GetValue();
			iCurVertex++;
		}

		m_bUpdateVertexBuffer = false;

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		Application::GetInstance()->GetDeviceContext()->Map(m_pd3dVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, pVertices, sizeof(VertexPosColor) * iNumVertices);
		Application::GetInstance()->GetDeviceContext()->Unmap(m_pd3dVertexBuffer, 0);

		Application::GetInstance()->ReRender();
	}

	PrepareViewAndRotationMatrix(fDeltaTime);

	return iRet;
}

int EngineRenderComponent::Render()
{
	int iRet = Component::Render();

	//Set the default blend state (no blending) for opaque objects
	Application::GetInstance()->GetDeviceContext()->OMSetBlendState(0, 0, 0xffffffff);

	//Set the blend state for transparent objects
	if (m_fAlpha < 1.0f)
	{
		// Set the blending equation
		float blendFactor[] = { Application::GetInstance()->GetBackgroundColor().x, Application::GetInstance()->GetBackgroundColor().y, Application::GetInstance()->GetBackgroundColor().z, 1.0f - m_fAlpha };
		Application::GetInstance()->GetDeviceContext()->OMSetBlendState(m_pd3dBlendState, blendFactor, 0xffffffff);
	}

	// Create the view matrix 
	m_pViewMatrix = XMMatrixLookAtLH(Application::GetInstance()->GetMainCamera()->GetEyePosition(), Application::GetInstance()->GetMainCamera()->GetFocusPoint(), Application::GetInstance()->GetMainCamera()->GetUpDirection());

	Application::GetInstance()->GetDeviceContext()->UpdateSubresource(m_pd3dConstantBuffers[CB_Frame], 0, nullptr, &m_pViewMatrix, 0, 0); // The constant buffer per frame will now store the value of g_ViewMatrix

	Application::GetInstance()->GetDeviceContext()->UpdateSubresource(m_pd3dConstantBuffers[CB_Object], 0, nullptr, &m_pWorldMatrix, 0, 0);

	// Prepare input assembler for the rendering pipeline
	const UINT vertexStride = sizeof(VertexPosColor);
	const UINT offset = 0;

	// Set vertex and index buffer
	Application::GetInstance()->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer, &vertexStride, &offset); // Bind the vertex Buffer to Input Assembler stage
	Application::GetInstance()->GetDeviceContext()->IASetInputLayout(m_pd3dInputLayout);
	Application::GetInstance()->GetDeviceContext()->IASetIndexBuffer(m_pd3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);  // Bind the Index Buffer
	Application::GetInstance()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // Set what primitive shapes we are using to render, and currently we are using triangles

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

int EngineRenderComponent::Destroy()
{
	int iRet = Component::Destroy();

	// Remove and destroy vertices and indices
	m_lVertices->Clear();
	SafeReleaseTemplate(m_lVertices);

	m_lIndices->Clear();
	SafeReleaseTemplate(m_lIndices);

	SafeRelease(m_pd3dConstantBuffers[ConstantBuffer::CB_Application]);
	SafeRelease(m_pd3dConstantBuffers[ConstantBuffer::CB_Frame]);
	SafeRelease(m_pd3dConstantBuffers[ConstantBuffer::CB_Object]);
	SafeRelease(m_pd3dIndexBuffer);
	SafeRelease(m_pd3dVertexBuffer);
	SafeRelease(m_pd3dInputLayout);
	SafeRelease(m_pd3dVertexShader);
	SafeRelease(m_pd3dPixelShader);
	SafeRelease(m_pd3dBlendState);

	return iRet;
}

int EngineRenderComponent::BindVertexBuffer()
{
	// Create the buffer description
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// Convert the list of vertices into an array
	int iNumVertices = m_lVertices->GetNumElements();
	int iCurVertex = 0;
	VertexPosColor* pVertices = new VertexPosColor[iNumVertices];

	for (Iterator<VertexPosColor>* it = m_lVertices->GetHead(); it != NULL; it = it->GetNext())
	{
		pVertices[iCurVertex] = *it->GetValue();
		iCurVertex++;
	}

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * iNumVertices;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	// Get the buffer resource data to be used to create a buffer
	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = pVertices;

	// Create vertex buffer which can be bound to the input-assembler stage to render the cube geometry
	HRESULT hr = Application::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, &resourceData, &m_pd3dVertexBuffer);
	return hr;
}

int EngineRenderComponent::BindIndexBuffer()
{
	// Create and initialize the index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	// Convert the list of vertices into an array
	int iNumIndices = m_lIndices->GetNumElements();
	int iCurIndex = 0;
	WORD* pIndices = new WORD[iNumIndices];

	for (Iterator<int>* it = m_lIndices->GetHead(); it != NULL; it = it->GetNext())
	{
		pIndices[iCurIndex] = *it->GetValue();
		iCurIndex++;
	}

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(WORD) * iNumIndices;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	// Get the buffer resource data to be used to create a buffer
	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	resourceData.pSysMem = pIndices;

	HRESULT hr = Application::GetInstance()->GetDevice()->CreateBuffer(&indexBufferDesc, &resourceData, &m_pd3dIndexBuffer);
	return hr;
}

int EngineRenderComponent::BindConstantBuffer()
{
	// Create the constant buffers for the variables defined in the vertex shader.
	// Constant buffer is used to store shader variables that are constant per application, per frame or per object
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	// Create the buffer for each category: Per Application, Per Frame and Per Object
	HRESULT hr = Application::GetInstance()->GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_pd3dConstantBuffers[CB_Application]);
	if (FAILED(hr))
	{
		return -1;
	}
	hr = Application::GetInstance()->GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_pd3dConstantBuffers[CB_Frame]);
	if (FAILED(hr))
	{
		return -1;
	}
	hr = Application::GetInstance()->GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_pd3dConstantBuffers[CB_Object]);
	if (FAILED(hr))
	{
		return -1;
	}

	// Set up projection matrix
	// Build a left-handed projection matrix based on a field of view
	m_pProjectionMatrix = Application::GetInstance()->GetMainCamera()->GetProjectionMatrix();

	Application::GetInstance()->GetDeviceContext()->UpdateSubresource(m_pd3dConstantBuffers[CB_Application], 0, nullptr, &m_pProjectionMatrix, 0, 0); // For a shader-constant buffer; set pDstBox to NULL

	return 0;
}

int EngineRenderComponent::BindVertexShader()
{
	// Load the compiled vertex shader.
	ID3DBlob* vertexShaderBlob;
#if _DEBUG
	LPCWSTR compiledVertexShaderObject = L"SimpleVertexShader_d.cso";
#else
	LPCWSTR compiledVertexShaderObject = L"SimpleVertexShader.cso";
#endif

	HRESULT hr = D3DReadFileToBlob(compiledVertexShaderObject, &vertexShaderBlob);
	if (FAILED(hr))
	{
		return false;
	}

	hr = Application::GetInstance()->GetDevice()->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &m_pd3dVertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	// Create the input layout for the vertex shader.
	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = Application::GetInstance()->GetDevice()->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_pd3dInputLayout);
	if (FAILED(hr))
	{
		return false;
	}

	SafeRelease(vertexShaderBlob);
}

int EngineRenderComponent::BindPixelShader()
{
	// Load the compiled pixel shader.
	ID3DBlob* pixelShaderBlob;
#if _DEBUG
	LPCWSTR compiledPixelShaderObject = L"SimplePixelShader_d.cso";
#else
	LPCWSTR compiledPixelShaderObject = L"SimplePixelShader.cso";
#endif

	HRESULT hr = D3DReadFileToBlob(compiledPixelShaderObject, &pixelShaderBlob);
	if (FAILED(hr))
	{
		return hr;
	}

	hr = Application::GetInstance()->GetDevice()->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &m_pd3dPixelShader);
	if (FAILED(hr))
	{
		return hr;
	}

	SafeRelease(pixelShaderBlob);

	return hr;
}

void EngineRenderComponent::PrepareViewAndRotationMatrix(float fDeltaTime)
{
	// Set up rotation matrix
	XMVECTOR rotationAxis = XMVectorSet(0, 1, 0, 0);
	XMMATRIX scalingMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX rotMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(m_fAngle));

	m_pWorldMatrix *= scalingMatrix;
	m_pWorldMatrix *= rotMatrix;
}

void EngineRenderComponent::SetupVerticesAndIndices()
{
	if (m_bUpdateVertexBuffer)
	{
		m_lVertices->Clear();
	}

	XMFLOAT3 vPos = m_pOwner->GetWorldPosition() + m_vLocalPos;
	m_vSize = XMFLOAT3(2.0f, 2.0f, 2.0f);
	m_vLocalPos = XMFLOAT3(0.f, 0.f, 0.f);
	m_vColor = XMFLOAT3(1.f, 0.f, 0.f);

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

void EngineRenderComponent::SetupBlendState()
{
	//Define the Blending Equation
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	Application::GetInstance()->GetDevice()->CreateBlendState(&blendDesc, &m_pd3dBlendState);
}

void EngineRenderComponent::UpdateTranslation(XMFLOAT3 translation)
{
	m_pWorldMatrix *= XMMatrixTranslation(translation.x, translation.y, translation.z);
	Application::GetInstance()->ReRender();
}

