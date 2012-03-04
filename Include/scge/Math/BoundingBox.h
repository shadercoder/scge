#ifndef __BoundingBox_h__
#define __BoundingBox_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

/**
 * Axis-aligned Bounding Box.
 */
class BoundingBox
{
public:

	BoundingBox();
	BoundingBox(const Vector3& min, const Vector3& max);
	BoundingBox(const BoundingBox& box);

	// Expands the bounding box to contain the vertex.
	void add(const Vector3& vertex);

	// Expands the bounding box to contain the bounding box.
	void add(const BoundingBox& box);

	// Resets the bounding box.
	void reset();

	// Sets the box to zero.
	void setZero();

	// Gets if the box is infinite.
	bool isInfinite() const;

	// Gets the center point of the bounding box.
	Vector3 getCenter() const;

	// Gets the chosen corner vertice.
	Vector3 getCorner(int index) const;

	// Intersection with a ray.
	bool intersects(const Ray& ray, float& distance) const;
	
	// Transforms the BoundingBox by a matrix.
	BoundingBox transform(const Matrix4& mat) const;

public:

	Vector3 min;
	Vector3 max;
};

//-----------------------------------//

#endif // __BoundingBox_h__