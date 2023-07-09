#pragma once
#include "EngineComponent.h"

struct AABB
{
	float maxX, minX, maxY, minY, maxZ, minZ;

	bool IsColliding(AABB collisionBox)
	{
		return (
			minX <= collisionBox.maxX
			&& maxX >= collisionBox.minX
			&& minY <= collisionBox.maxY
			&& maxY >= collisionBox.minY
			&& minZ <= collisionBox.maxZ
			&& maxZ >= collisionBox.minZ
		);
	}
};

class EngineBoundingBoxComponent : public Component
{
public:
	EngineBoundingBoxComponent(bool bIsFit = true);

	// --------------- Required for every component --------//
	static Component* Create() { return new EngineBoundingBoxComponent(); }
	static void RegisterGameClass();
	virtual const char* GetComponentClassName() override { return "EngineBoundingBoxComponent"; }
	virtual int GetComponentClassNameHash() override { return ToStringHash("EngineBoundingBoxComponent"); }

	virtual bool ReadSaveData(rapidjson::Value::ConstValueIterator& itr) override;
	virtual bool WriteSaveData(rapidjson::Writer<rapidjson::StringBuffer>* writer) override;
	// -------------------------------------------------//

	virtual int Initialize() override;
	virtual int Update(float fDeltaTime) override;

	virtual void SetIsFit(bool bIsFit) { m_bIsFit = bIsFit; }

	AABB GetAABB();

protected:
	bool m_bIsFit;
	AABB m_AABB;
};