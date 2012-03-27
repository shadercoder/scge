#ifndef __Plane_h__
#define __Plane_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

class Plane
{
public:
	Plane();
	Plane(const Vector3& normal, const Vector3& point);
	Plane(const Vector3& normal, float distance);

	void normalise();

	float distance(const Vector3& point) const;
	float distance(float x, float y, float z) const;

	Vector3 project(const Vector3& vec) const;

	IntersectionType intersects(const Vector3& point) const;
	bool intersects(const Ray& ray, float& distance) const;

	Vector3 normal;
	float offset;
};

//-----------------------------------//

#endif // __Plane_h__