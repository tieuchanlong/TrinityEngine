#pragma once
#include "EngineRenderComponent.h"

using namespace DirectX;

class EngineRectangleComponent : public EngineRenderComponent
{
public:
	EngineRectangleComponent();
	EngineRectangleComponent(XMFLOAT3 vPos, XMFLOAT3 vSize);
	EngineRectangleComponent(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor);

	// --------------- Required for every component --------//
	static Component* Create() { return new EngineRectangleComponent(); }
	static void RegisterGameClass();
	virtual const char* GetComponentClassName() override { return "EngineRectangleComponent"; }
	virtual int GetComponentClassNameHash() override { return ToStringHash("EngineRectangleComponent"); }
	// -------------------------------------------------//

	virtual int Initialize() override;
	virtual int PreUpdate() override;
	virtual int Render() override;
protected:
	XMVECTOR m_vOriginalDistFromCameraFocus;

	virtual void SetupVerticesAndIndices() override;
	virtual void PrepareViewAndRotationMatrix(float fDeltaTime) override;
};