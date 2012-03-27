#ifndef __BoundingBox_h__
#define __BoundingBox_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const Vector3& min, const Vector3& max);
	BoundingBox(const BoundingBox& box);

	void add(const Vector3& vertex);
	void add(const BoundingBox& box);

	void reset();

	void setZero();

	bool isInfinite() const;

	Vector3 getCenter() const;
	Vector3 getCorner(int index) const;

	bool intersects(const Ray& ray, float& distance) const;
	
	BoundingBox transform(const Matrix4& mat) const;

	Vector3 min;
	Vector3 max;
};

//-----------------------------------//

#endif // __BoundingBox_h__