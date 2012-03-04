#ifndef __Ray_h__
#define __Ray_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

/**
 * Ray class.
 */
class Ray
{
public:

	Ray();
	Ray(const Vector3& origin, const Vector3& direction);
	Ray(const Ray& ray );

	// Gets a point on the ray.
	Vector3 getPoint(float distance) const;

	// Intersects the ray with a triangle.
	bool intersects(const Vector3 tri[3], float& t, float& u, float& v) const;

public:

	// Point representing the origin of the ray.
	Vector3 origin;

	// Vector representing the direction of the ray.
	Vector3 direction;
};

//-----------------------------------//

#endif // __Ray_h__