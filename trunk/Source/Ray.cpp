#include "scge\Math\Ray.h"

//-----------------------------------//

Ray::Ray() {}

//-----------------------------------//

Ray::Ray( const Vector3& origin, const Vector3& direction ) 
	: origin( origin )
	, direction( direction )
{ }

//-----------------------------------//

Ray::Ray( const Ray& ray )
	: origin( ray.origin )
	, direction( ray.direction )
{ }

//-----------------------------------//

Vector3 Ray::getPoint(float distance) const
{
	return origin + direction*distance;
}

//-----------------------------------//

bool Ray::intersects(const Vector3 tri[3], float& t, float& u, float& v) const 
{ 
	Vector3 v1 = tri[0];
	Vector3 v2 = tri[1];
	Vector3 v3 = tri[2];

	Vector3 edge1 = v2-v1;
	Vector3 edge2 = v3-v1;
	Vector3 pvec = direction.crossProduct(edge2);

	float det = edge1.dotProduct(pvec);

	if(det > -Math::Limits::FloatEpsilon && det < Math::Limits::FloatEpsilon)
		return false;

	float inv_det = 1.0f / det;
	Vector3 tvec = origin-v1;
	u = tvec.dotProduct(pvec) * inv_det;

	if(u < 0.0 || u > 1.0)
		return false;

	Vector3 qvec = tvec.crossProduct(edge1);
	v = direction.dotProduct(qvec) * inv_det;

	if(v < 0.0 || (u + v) > 1.0)
		return false;

	t = edge2.dotProduct(qvec) * inv_det;

	return true;
}

//-----------------------------------//