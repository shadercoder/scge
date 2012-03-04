#ifndef __BoundingSphere_h__
#define __BoundingSphere_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

/**
 * Bounding Sphere.
 */

class BoundingSphere
{
public:

	BoundingSphere();
	BoundingSphere(const Vector3& center, float radius);
	BoundingSphere(const BoundingSphere& bs);
	BoundingSphere(const BoundingBox& box);

	// Grow sphere to include point.
	void add(const Vector3& vertex);

	// Intersection with a ray.
	bool intersects(const Ray& ray, float& distance) const;

	// Intersection with a sphere.
	bool intersects(const BoundingSphere& sphere) const;

	// Intersection with a point.
	bool intersects(const Vector3& pt) const;
	
public:

	Vector3 center;
	float radius;
};

//-----------------------------------//

#endif // __BoundingSphere_h__