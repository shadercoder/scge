#ifndef __Ray_h__
#define __Ray_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

class Ray
{
public:
	Ray();
	Ray(const Vector3& origin, const Vector3& direction);
	Ray(const Ray& ray);

	Vector3 getPoint(float distance) const;

	bool intersects(const Vector3 tri[3], float& t, float& u, float& v) const;

	Vector3 origin;
	Vector3 direction;
};

//-----------------------------------//

#endif // __Ray_h__