#include "scge\Math\Plane.h"

#include "scge\Math\Vector3.h"
#include "scge\Math\Ray.h"

//-----------------------------------//

Plane::Plane()
	: normal()
	, offset()
{ }

//-----------------------------------//

Plane::Plane(const Vector3& _normal, const Vector3& point)
	: normal(_normal)
	, offset()
{
	normal.Normalise();
	offset = -normal.dotProduct(point);
}

//-----------------------------------//

Plane::Plane(const Vector3& _normal, float distance)
	: normal(_normal)
	, offset(distance)
{
	normal.Normalise();
}

//-----------------------------------//

float Plane::distance(const Vector3& pt) const
{
	return distance(pt.x, pt.y, pt.z);
}

//-----------------------------------//

float Plane::distance(float x, float y, float z) const
{
	return normal.x*x + normal.y*y + normal.z*z	+ offset;
}

//-----------------------------------//

Vector3 Plane::project(const Vector3& vec) const
{
	return vec - normal*vec.dotProduct(normal);
}

//-----------------------------------//

IntersectionType Plane::intersects(const Vector3& point) const
{
	float fDistance = distance(point);

	if(fDistance < Math::Limits::FloatEpsilon)
		return IntersectionType::NegativeSide;
	else if(fDistance > Math::Limits::FloatEpsilon)
		return IntersectionType::PositiveSide;
	
	return IntersectionType::Intersects;
}

//-----------------------------------//

bool Plane::intersects(const Ray& ray, float& distance) const
{
	// Gets the angle between the ray and the plane normal.
	float angle = ray.direction.dotProduct(normal);

	// If they are perpendicular, then they will never intersect.
	if(Math::Equal(angle, 0))
		return false;

	float delta = offset - ray.origin.dotProduct(normal);
	distance = delta / angle;

	return true;
}

//-----------------------------------//

void Plane::normalise()
{
	float magnitude = normal.Normalise();

	offset /= magnitude;
}

//-----------------------------------//