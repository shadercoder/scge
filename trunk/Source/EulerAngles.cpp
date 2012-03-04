#include "scge\Math\EulerAngles.h"

#include "scge\Math\Vector3.h"
#include "scge\Math\Matrix4.h"

//-----------------------------------//

EulerAngles::EulerAngles()
	: x(0.0f)
	, y(0.0f)
	, z(0.0f)
{ }

//-----------------------------------//

EulerAngles::EulerAngles( float x, float y, float z )
	: x(x)
	, y(y)
	, z(z)
{ }

//-----------------------------------//

EulerAngles::EulerAngles( const EulerAngles& ang )
	: x(ang.x)
	, y(ang.y)
	, z(ang.z)
{ }

EulerAngles::EulerAngles(const Quaternion& quat)
{
	Vector3 forward = Vector3::UnitZ * quat;
	Vector3 up = Vector3::UnitY * quat;

	EulerAngles rotationaxes = PointAt(Vector3(0.0f), forward);

	if(rotationaxes.x == Math::Constants::PiOverTwo)
	{
		rotationaxes.y = Math::ArcTanAngle(up.z, up.x);
		rotationaxes.z = 0;
	}
	else if(rotationaxes.x == -Math::Constants::PiOverTwo)
	{
		rotationaxes.y = Math::ArcTanAngle(-up.z, -up.x);
		rotationaxes.y = 0;
	}
	else
	{
		up = up * Math::CreateRotationMatrixY(-rotationaxes.y);
		up = up * Math::CreateRotationMatrixX(-rotationaxes.x);
		rotationaxes.z = Math::ArcTanAngle(up.y, -up.x);
	}
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

EulerAngles EulerAngles::PointAt(const Vector3& from, const Vector3& location)
{
	EulerAngles ret;

	Vector3 v = (location - from).Normalise();

	ret.x = Math::ASin(v.y);
	ret.y = Math::ArcTanAngle(-v.z, -v.x);
	ret.z = 0.0f;

	return ret;
}