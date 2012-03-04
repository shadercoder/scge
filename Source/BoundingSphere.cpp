#include "scge\Math\BoundingSphere.h"

#include "scge\Math\BoundingBox.h"

//-----------------------------------//

BoundingSphere::BoundingSphere()
	: center()
	, radius()
{ }

//-----------------------------------//

BoundingSphere::BoundingSphere(const Vector3& center, float radius)
	: center( center )
	, radius( radius )
{ }

//-----------------------------------//

BoundingSphere::BoundingSphere(const BoundingSphere& bs)
	: center( bs.center )
	, radius( bs.radius )
{ }

//-----------------------------------//

BoundingSphere::BoundingSphere(const BoundingBox& box)
	: center( box.getCenter() )
	, radius( (box.getCenter() - box.max.x).length() )
{ }

//-----------------------------------//

void BoundingSphere::add(const Vector3& vertex)
{
	float dist = center.distance(vertex);
	if(dist > radius)
		radius = dist;
}

//-----------------------------------//

bool BoundingSphere::intersects(const Ray& ray, float& distance) const
{
	//TODO : implement
	return false;
}

//-----------------------------------//

bool BoundingSphere::intersects(const BoundingSphere& sphere) const
{
	Vector3 dist = center - sphere.center;
	float minDist = radius + sphere.radius;

	return dist.squaredLength() <= minDist * minDist;
}

//-----------------------------------//

bool BoundingSphere::intersects(const Vector3& pt) const
{
	Vector3 v = center - pt;

	if( v.squaredLength() > radius*radius )
		return false;

	return true;
}

//-----------------------------------//