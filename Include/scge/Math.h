#ifndef __Math_h__
#define __Math_h__

#include <limits>
#include <algorithm>
#include <ostream>
#include <cmath>

//-----------------------------------//

template <typename T>
class Vector2T;
template <typename T>
class Vector3T;
template <typename T>
class Vector4T;

typedef Vector2T<float> Vector2;
typedef Vector2T<int> Vector2i;

typedef Vector3T<int> Vector3i;
typedef Vector3T<float> Vector3;
typedef Vector3T<double> Vector3d;

typedef Vector4T<int> Vector4i;
typedef Vector4T<float> Vector4;
typedef Vector4T<double> Vector4d;

class Matrix3;
class Matrix4;

class Quaternion;

class Plane;
class Frustum;
class Ray;
class EulerAngles;
class BoundingBox;
class BoundingSphere;
class Colour;

//-----------------------------------//

enum class IntersectionType
{
	Inside,
	Intersects,
	Outside,
	NegativeSide,
	PositiveSide
};

//-----------------------------------//

namespace Math
{
	namespace Limits
	{
		const float FloatMinimum = std::numeric_limits<float>::min();
		const float FloatMaximum = std::numeric_limits<float>::max();
		const float FloatEpsilon = std::numeric_limits<float>::epsilon();

		const float PositiveInfinty = std::numeric_limits<float>::infinity();
		const float NegativeInfinity = -std::numeric_limits<float>::infinity();
	}

	namespace Constants
	{
		const float Pi = 4.0f * atanf(1.0f);
		const float TwoPi = 2.0f * Pi;
		const float HalfPi = 0.5f * Pi;
		const float PiOverTwo = Pi / 2;
		const float Deg2Rad =  Pi / 180.0f;
		const float Rad2Deg = 180.0f / Pi;
		const float Log2 = logf(2.0f);
	}

	namespace Interpolate
	{
		float linear(float a, float b, float t);
		float cosine(float a, float b, float t);
	}

	inline bool isNaN(float n) { return n != n; }

	inline float Abs(float fValue) { return fabs(fValue); }
	inline float Ceil(float fValue) { return ceilf(fValue); }
	inline float Floor(float fValue) { return floorf(fValue); }
	inline float Sign(float fValue) { return(fValue > 0.0f ? 1.0f : (fValue < 0.0f ? -1.0f : 0.0f)); }

	inline int IAbs(int iValue) { return ( iValue >= 0 ? iValue : -iValue ); }
	inline int ICeil(float fValue) { return int(Ceil(fValue)); }
	inline int IFloor(float fValue) { return int(Floor(fValue)); }
	inline int ISign(int iValue) { return(iValue > 0 ? +1 : (iValue < 0 ? -1 : 0)); }

	inline float ACos(float fValue) { return acosf(fValue); }
	inline float ASin(float fValue) { return asinf(fValue); }
	inline float ATan(float fValue) { return atanf(fValue); }
	inline float ATan2(float fY, float fX) { return atan2f(fY, fX); }

	inline float Exp(float fValue) { return expf(fValue); }
	inline float Log(float fValue) { return logf(fValue); }

	inline float Log2(float fValue) { return logf(fValue) / Constants::Log2; }
	inline float LogN(float base, float fValue) { return logf(fValue) / logf(base); }
	inline float Pow(float fBase, float fExponent) { return powf(fBase, fExponent); }

	inline float Sin(float fValue) { return sinf(fValue); }
	inline float Cos(float fValue) { return cosf(fValue); }
	inline float Tan(float fValue) { return tanf(fValue); }
	inline float Cot(float fValue) { return(1 / Tan(fValue)); }

	inline float Sqr(float fValue) { return fValue * fValue; }
	inline float Sqrt(float fValue) { return sqrtf(fValue); }
	inline float InvSqrt(float fValue) { return 1.0f / Sqrt(fValue); }

	void seedRandom(unsigned long seedValue);
	void seedRandomTime();

	float RangeRandom(float low, float high);
	int RangeRandom(int low, int high);
	unsigned int RangeRandom(unsigned int low, unsigned int high);
	inline float UnitRandom() { return RangeRandom(0.0f, 1.0f); }
	inline float SymmetricRandom() { return RangeRandom(-1.0f, 1.0f); }

	// Compares float values for equality.
	inline bool Equal(float a, float b, float tolerance = Limits::FloatEpsilon) { return Abs(a - b) < tolerance; }

	// Calculates the logarithm of a given base.
	inline float logBase(float base, float value) { return log(value) / log(base); }

	inline float DegreesToRadians(float degrees) { return degrees * Constants::Deg2Rad; }
	inline float RadiansToDegrees(float radians) { return radians * Constants::Rad2Deg; }

	// Clamps a variable to the given range.
	template<typename T>
	inline T clamp(const T var, const T min = 0, const T max = 1)
	{
		return var >= max ? max : (var <= min ? min : var);
	}

	float ArcTanAngle(const float& x, const float& y);

	Vector3 CalculateTriangleNormal(const Vector3& v1, const Vector3& v2, const Vector3& v3);

	Matrix4 CreatePerspectiveProjection(float fFOV, float fAspect, float fNear, float fFar);
	Matrix4 CreateOrthographicProjection(float fWidth, float fHeight, float fNear, float fFar);

	Matrix3 CreateRotationMatrixX(float fValue);
	Matrix3 CreateRotationMatrixY(float fValue);
	Matrix3 CreateRotationMatrixZ(float fValue);
}

//-----------------------------------//

#endif // __Math_h__