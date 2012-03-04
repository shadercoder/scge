#ifndef __EulerAngles_h__
#define __EulerAngles_h__

#include "scge\Math.h"

//-----------------------------------//

/**
 * Representation of an orientation as a set of Euler angles that can be
 * transformed into and back in a rotation matrix and quaternions.
 */
class EulerAngles
{
public:

	EulerAngles();
	EulerAngles(float x, float y, float z);
	EulerAngles(const Quaternion& quat);

	// Copy constructor.
	EulerAngles(const EulerAngles&);

	// Addition combined.
	EulerAngles& operator+=(const EulerAngles&);

	// Addition combined.
	EulerAngles& operator+=(const Vector3&);

	EulerAngles operator+(const EulerAngles& a) const;

	// Adds the given angles to the existing angles.
	void rotate(float x, float y, float z);

	// Resets the rotations so they don't have any effect.
	void identity();

	// Create EulerAngles that represent the direction from one point to anouther
	static EulerAngles PointAt(const Vector3& from, const Vector3& location);

public:

	float x, y, z;
};

//-----------------------------------//

#endif // __EulerAngles_h__