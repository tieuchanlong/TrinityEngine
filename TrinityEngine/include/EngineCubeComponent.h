#pragma once
#include "EngineRenderComponent.h"

using namespace DirectX;

class EngineCubeComponent : public EngineRenderComponent
{
public:
	EngineCubeComponent();
	EngineCubeComponent(XMFLOAT3 vPos, XMFLOAT3 vSize);
	EngineCubeComponent(XMFLOAT3 vPos, XMFLOAT3 vSize, XMFLOAT3 vColor);

	virtual int Initialize() override;

	// --------------- Required for every component --------//
	static Component* Create() { return new EngineCubeComponent(); }
	static void RegisterGameClass();
	virtual const char* GetComponentClassName() override { return "EngineCubeComponent"; }
	virtual int GetComponentClassNameHash() override { return ToStringHash("EngineCubeComponent"); }
	// -------------------------------------------------//
protected:
	virtual void SetupVerticesAndIndices() override;
};