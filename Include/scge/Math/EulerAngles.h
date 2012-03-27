#ifndef __EulerAngles_h__
#define __EulerAngles_h__

#include "scge\Math.h"

//-----------------------------------//

class EulerAngles
{
public:
	EulerAngles();
	EulerAngles(float x, float y, float z);
	EulerAngles(const Quaternion& quat);
	EulerAngles(const EulerAngles& ang);
	EulerAngles(const Vector3& from, const Vector3& location);

	EulerAngles& operator+=(const EulerAngles&);
	EulerAngles& operator+=(const Vector3&);
	EulerAngles operator+(const EulerAngles& a) const;

	void rotate(float x, float y, float z);

	void identity();

	float x, y, z;
};

//-----------------------------------//

#endif // __EulerAngles_h__