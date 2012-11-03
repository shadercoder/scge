#include "scge\Math.h"

#include "scge\Math\Vector3.h"
#include "scge\Math\Matrix4.h"
#include "scge\Math\Matrix3.h"

#include <random>
#include <ctime>

//-----------------------------------//

float Math::Interpolate::linear(float a, float b, float t)
{
	// Linear interpolation.
	return a*(1-t) + b*t;
}

//-----------------------------------//

float Math::Interpolate::cosine(float a, float b, float t)
{
	// Cosine interpolation.
	float ft = t * Math::Constants::Pi;
	float f = (1 - Math::Cos(ft)) * 0.5f;

	// Linear interpolation.
	return linear(a, b, f);
}

//-----------------------------------//

static std::mt19937 randomEngine;

void Math::seedRandom(unsigned long seedValue)
{
	randomEngine.seed(seedValue);
}

void Math::seedRandomTime()
{
	randomEngine.seed(static_cast<unsigned long>(std::time(nullptr)));
}

float Math::RangeRandom(float low, float high)
{
	return std::uniform_real_distribution<float>(low, high)(randomEngine);
}

int Math::RangeRandom(int low, int high)
{
	return std::uniform_int_distribution<int>(low, high)(randomEngine);
}

unsigned int Math::RangeRandom(unsigned int low, unsigned int high)
{
	return std::uniform_int_distribution<unsigned int>(low, high)(randomEngine);
}

//-----------------------------------//

float Math::ArcTanAngle(const float& x, const float& y)
{
	if(x == 0)
	{
		if(y == 1)
			return Constants::PiOverTwo;
		else
			return -Constants::PiOverTwo;
	}
	else if(x > 0)
		return ATan(y / x);
	else if(x < 0)
	{
		if(y > 0)
			return ATan(y / x) + Constants::Pi;
		else
			return ATan(y / x) - Constants::Pi;
	}
	else
		return 0;
}

//-----------------------------------//

Vector3 Math::CalculateTriangleNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3)
{
	Vector3 vec1 = v2 - v1;
	Vector3 vec2 = v3 - v1;
	Vector3 normal = vec1.crossProduct(vec2);

	return normal.Normalise();
}

//-----------------------------------//

Matrix4 Math::CreatePerspectiveProjection(float fFOV, float fAspect, float fNear, float fFar)
{
	Matrix4 ret = Matrix4::Zero;

	float yScale = Cot(fFOV/2);
	float xScale = yScale / fAspect;

	ret[0][0] = xScale;
	ret[1][1] = yScale;
	ret[2][2] = fFar / (fFar - fNear);
	ret[2][3] = 1.0f;
	ret[3][2] = -fNear * fFar / (fFar - fNear);

	return ret;
}

//-----------------------------------//

Matrix4 Math::CreateOrthographicProjection(float fWidth, float fHeight, float fNear, float fFar)
{
	Matrix4 ret = Matrix4::Zero;

	ret[0][0] = 2 / fWidth;
	ret[1][1] = 2 / fHeight;
	ret[2][2] = 1.0f / (fFar - fNear);
	ret[3][2] = -fNear / (fFar - fNear);
	ret[3][3] = 1.0f;

	return ret;
}

//-----------------------------------//

Matrix3 Math::CreateRotationMatrixX(float fValue)
{
	Matrix3 ret = Matrix3::Identity;

	ret[1][1] = Cos(fValue);
	ret[2][2] = Cos(fValue);

	ret[1][2] = Sin(fValue);
	ret[2][1] = -Sin(fValue);

	return ret;
}

//-----------------------------------//

Matrix3 Math::CreateRotationMatrixY(float fValue)
{
	Matrix3 ret = Matrix3::Identity;

	ret[0][0] = Cos(fValue);
	ret[2][2] = Cos(fValue);

	ret[0][2] = -Sin(fValue);
	ret[2][0] = Sin(fValue);

	return ret;
}

//-----------------------------------//

Matrix3 Math::CreateRotationMatrixZ(float fValue)
{
	Matrix3 ret = Matrix3::Identity;

	ret[0][0] = Cos(fValue);
	ret[1][1] = Cos(fValue);

	ret[1][0] = -Sin(fValue);
	ret[0][1] = Sin(fValue);

	return ret;
}

//-----------------------------------//
