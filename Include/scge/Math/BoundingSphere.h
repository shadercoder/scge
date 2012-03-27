#ifndef __BoundingSphere_h__
#define __BoundingSphere_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

class BoundingSphere
{
public:
	BoundingSphere();
	BoundingSphere(const Vector3& center, float radius);
	BoundingSphere(const BoundingSphere& bs);
	BoundingSphere(const BoundingBox& box);

	void add(const Vector3& vertex);

	bool intersects(const BoundingSphere& sphere) const;
	bool intersects(const Vector3& pt) const;

	Vector3 center;
	float radius;
};

//-----------------------------------//

#endif // __BoundingSphere_h__