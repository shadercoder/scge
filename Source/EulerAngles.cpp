#include "scge\Math\EulerAngles.h"

#include "scge\Math\Vector3.h"
#include "scge\Math\Quaternion.h"
#include "scge\Math\Matrix3.h"

//-----------------------------------//

EulerAngles::EulerAngles()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
{ }

//-----------------------------------//

EulerAngles::EulerAngles(float x, float y, float z)
	: x(x)
	, y(y)
	, z(z)
{ }

//-----------------------------------//

EulerAngles::EulerAngles(const EulerAngles& ang)
	: x(ang.x)
	, y(ang.y)
	, z(ang.z)
{ }

//-----------------------------------//

EulerAngles::EulerAngles(const Quaternion& quat)
{
	Vector3 forward = Vector3::UnitZ * quat;
	Vector3 up = Vector3::UnitY * quat;

	*this = EulerAngles(Vector3(0.0f), forward);
	
	if(x == Math::Constants::PiOverTwo)
	{
		y = Math::ArcTanAngle(up.z, up.x);
		z = 0;
	}
	else if(x == -Math::Constants::PiOverTwo)
	{
		y = Math::ArcTanAngle(-up.z, -up.x);
		y = 0;
	}
	else
	{
		up = up * Math::CreateRotationMatrixY(-y);
		up = up * Math::CreateRotationMatrixX(-x);
		z = Math::ArcTanAngle(up.y, -up.x);
	}
}

//-----------------------------------//

EulerAngles::EulerAngles(const Vector3& from, const Vector3& location)
{
	Vector3 v = (location - from).Normalise();

	x = Math::ASin(v.y);
	y = Math::ArcTanAngle(-v.z, -v.x);
	z = 0.0f;
}

//-----------------------------------//

EulerAngles& EulerAngles::operator+=(const EulerAngles& ang)
{
	x += ang.x;
	y += ang.y;
	z += ang.z;

	return *this;
}

//-----------------------------------//

EulerAngles& EulerAngles::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;

	return *this;
}

//-----------------------------------//

void EulerAngles::rotate(float x, float y, float z)
{
	x += x;
	y += y;
	z += z;
}

//-----------------------------------//

void EulerAngles::identity()
{
	x = y = z = 0;
}

//-----------------------------------//

EulerAngles EulerAngles::operator+(const EulerAngles& a) const
{
	return EulerAngles(x+a.x, y+a.y, z+a.z);
}

//-----------------------------------//