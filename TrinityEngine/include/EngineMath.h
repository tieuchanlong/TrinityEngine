#pragma once
#include <DirectXMath.h> // include primitive math libraries

using namespace DirectX;

#define PI 3.14159265

struct Vector2
{
	Vector2() { x = y = 0; }
	Vector2(float fX, float fY) { x = fX; y = fY; }

	float x;
	float y;
};

struct Vector3
{
	Vector3() { x = y = z = 0; }
	Vector3(float fX, float fY, float fZ) { x = fX; y = fY; z = fZ; }

	float x;
	float y;
	float z;
};

struct Vector4
{
	Vector4() { a = b = c = d = 0; }
	Vector4(float fA, float fB, float fC, float fD) { a = fA; b = fB; c = fC; d = fD; }

	float a;
	float b;
	float c;
	float d;
};

static XMFLOAT3 operator * (float s, XMFLOAT3 vVector)
{
	XMFLOAT3 vOut;
	vOut.x = vVector.x * s;
	vOut.y = vVector.y * s;
	vOut.z = vVector.z * s;

	return vOut;
}

static XMFLOAT3 operator + (XMFLOAT3 vVector1, XMFLOAT3 vVector2)
{
	XMFLOAT3 vOut;
	vOut.x = vVector1.x + vVector2.x;
	vOut.y = vVector1.y + vVector2.y;
	vOut.z = vVector1.z + vVector2.z;

	return vOut;
}

static XMFLOAT3 operator - (XMFLOAT3 vVector1, XMFLOAT3 vVector2)
{
	XMFLOAT3 vOut;
	vOut.x = vVector1.x - vVector2.x;
	vOut.y = vVector1.y - vVector2.y;
	vOut.z = vVector1.z - vVector2.z;

	return vOut;
}

static XMFLOAT3 operator / (XMFLOAT3 vVector, float s)
{
	XMFLOAT3 vOut;
	vOut.x = vVector.x / s;
	vOut.y = vVector.y / s;
	vOut.z = vVector.z / s;

	return vOut;
}

static bool operator != (XMFLOAT3 vVector1, XMFLOAT3 vVector2)
{
	return (vVector1.x != vVector2.x || vVector1.y != vVector2.y || vVector1.z != vVector2.z);
}

struct Utilities
{
	Utilities();

	/// <summary>
	/// Returns the rotated vector by certain angle around the origin and use target-origin as reference vector
	/// </summary>
	/// <param name="origin">origin of the vector</param>
	/// <param name="target">target of the reference vector</param>
	/// <param name="rotationAngle">rotational angle in right, up, and front direction</param>
	/// <returns></returns>
	static XMVECTOR GetRotatedVector(XMVECTOR origin, XMVECTOR target, XMVECTOR rotationAngle)
	{
		XMVECTOR rotatedVector = target - origin;
		float vectorX = XMVectorGetX(rotatedVector);
		float vectorY = XMVectorGetY(rotatedVector);
		float vectorZ = XMVectorGetZ(rotatedVector);

		float angleX = XMVectorGetX(rotationAngle);
		float angleY = XMVectorGetY(rotationAngle);
		float angleZ = XMVectorGetZ(rotationAngle);

		float vecDist = sqrt(vectorX * vectorX + vectorY * vectorY + vectorZ * vectorZ);

		// Rotation matrix about reference x axis
		XMMATRIX xRotationMatrix = XMMATRIX(1.f, 0.f, 0.f, 0.f,
			0.f, vecDist * cos(angleX * PI / 180.f), 0, 0,
			0.f, 0.f, vecDist * sin(angleX * PI / 180.f), 0.f,
			0.f, 0.f, 0.f, 0.f);

		// Rotation matrix about reference y axis
		XMMATRIX yRotationMatrix = XMMATRIX(vecDist * sin(angleY * PI / 180.f), 0.f, 0.f, 0.f,
			0.f, 0.f, 0, 0,
			0.f, 0.f, vecDist * cos(angleY * PI / 180.f), 0.f,
			0.f, 0.f, 0.f, 0.f);

		// Rotation matrix about reference z axis
		XMMATRIX zRotationMatrix = XMMATRIX(vecDist * cos(angleZ * PI / 180.f), 0.f, 0.f, 0.f,
			0.f, vecDist * sin(angleZ * PI / 180.f), 0, 0,
			0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 0.f, 0.f);

		return XMVector4Transform(rotatedVector, xRotationMatrix * yRotationMatrix * zRotationMatrix);
	}
};

// Comparison section
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))