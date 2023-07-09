#pragma once
#include "EngineComponent.h"
#include "EngineList.h"

using namespace DirectX;

// Vertex data for a colored cube.
struct VertexPosColor
{
	XMFLOAT3 Position;
	XMFLOAT3 Color;
};

class EngineRenderComponent : public Component
{
public:
	// Shader resources
	enum ConstantBuffer
	{
		CB_Application,
		CB_Frame,
		CB_Object,
		NumConstantBuffers
	};

	EngineRenderComponent();

	// --------------- Required for every component --------//
	static Component* Create() { return new EngineRenderComponent(); }
	static void RegisterGameClass();
	virtual const char* GetComponentClassName() override { return "EngineRenderComponent"; }
	virtual int GetComponentClassNameHash() override { return ToStringHash("EngineRenderComponent"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr);
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer);
	// -------------------------------------------------//

	virtual int Initialize() override;
	virtual int Update(float fDeltaTime) override;
	virtual int Render() override;
	virtual int Destroy() override;

	virtual XMFLOAT3 GetLocalPosition() { return m_vLocalPos; }
	virtual XMFLOAT3 GetSize() { return m_vSize; }
	virtual XMFLOAT3 GetColor() { return m_vColor; }

	virtual void SetLocalPosition(XMFLOAT3 vLocalPos) {
		if (vLocalPos != m_vLocalPos)
		{
			m_bUpdateVertexBuffer = true;
		}
		m_vLocalPos = vLocalPos;
	}

	virtual void SetSize(XMFLOAT3 vSize) {
		if (vSize != m_vSize)
		{
			m_bUpdateVertexBuffer = true;
		}
		m_vSize = vSize;
	}

	virtual void SetColor(XMFLOAT3 vNewColor) {
		if (vNewColor != m_vColor)
		{
			m_bUpdateVertexBuffer = true;
		}
		m_vColor = vNewColor;
	}

	virtual void SetAlpha(float fAlpha)
	{
		m_fAlpha = fAlpha;
	}

	virtual void UpdateTranslation(XMFLOAT3 translation);

	virtual XMFLOAT3 GetMinDim() { return m_vLocalPos - m_vSize / 2; }
	virtual XMFLOAT3 GetMaxDim() { return m_vLocalPos + m_vSize / 2; }

protected:
	List<VertexPosColor> * m_lVertices;
	List<int> * m_lIndices;

	ID3D11InputLayout* m_pd3dInputLayout = nullptr;
	// Vertex buffer data
	ID3D11Buffer* m_pd3dVertexBuffer = nullptr;
	// Index buffer data
	ID3D11Buffer* m_pd3dIndexBuffer = nullptr;
	// Array contains constant buffer for each category
	ID3D11Buffer* m_pd3dConstantBuffers[NumConstantBuffers];

	// Shader stuff
	ID3D11VertexShader* m_pd3dVertexShader = nullptr;
	ID3D11PixelShader* m_pd3dPixelShader = nullptr;

	// View and rotation matrix
	XMMATRIX m_pWorldMatrix; // 4x4 matrix will be used to store the world matrix of the cube into the scene
	XMMATRIX m_pViewMatrix; // Needs to be updated once per scene. Store camera view matrix to transform object's vertices from world space into view space
	XMMATRIX m_pProjectionMatrix; // Store projection matrix into the camera. Transform the object vertices from view space into clip space

	XMFLOAT3 m_vLocalPos;
	XMFLOAT3 m_vSize;
	XMFLOAT3 m_vColor;
	float m_fAngle;
	float m_fAlpha;

	// Transparency
	ID3D11BlendState* m_pd3dBlendState;

	virtual int BindVertexBuffer();
	virtual int BindIndexBuffer();
	virtual int BindConstantBuffer();
	virtual int BindVertexShader();
	virtual int BindPixelShader();

	virtual void PrepareViewAndRotationMatrix(float fDeltaTime);
	virtual void SetupVerticesAndIndices();
	virtual void SetupBlendState();

	bool m_bUpdateVertexBuffer;
};