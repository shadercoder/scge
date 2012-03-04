#ifndef __Quaternion_h__
#define __Quaternion_h__

#include "scge\Math.h"

//-----------------------------------//

class Quaternion
{
public:
	// Default constructor, initializes to identity rotation (aka 0°)
	inline Quaternion ()
		: w(1), x(0), y(0), z(0)
	{
	}

	// Construct from an explicit list of values
	inline Quaternion (float fW, float fX, float fY, float fZ)
		: w(fW), x(fX), y(fY), z(fZ)
	{
	}

	// Construct a quaternion from a rotation matrix
	inline Quaternion(const Matrix3& rot)
	{
		FromRotationMatrix(rot);
	}

	// Construct a quaternion from an angle/axis
	inline Quaternion(const float& rfAngle, const Vector3& rkAxis)
	{
		FromAngleAxis(rfAngle, rkAxis);
	}

	// Construct a quaternion from 3 orthonormal local axes
	inline Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
	{
		FromAxes(xaxis, yaxis, zaxis);
	}

	inline void swap(Quaternion& other)
	{
		std::swap(w, other.w);
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
	}

	inline float operator[](const size_t i) const
	{
		return *(&w+i);
	}

	inline float& operator[](const size_t i)
	{
		return *(&w+i);
	}

	inline float *ptr()
	{
		return &w;
	}

	inline const float *ptr() const
	{
		return &w;
	}

	void FromRotationMatrix(const Matrix3& kRot);
	Matrix3 ToRotationMatrix() const;

	void FromAngleAxis(const float& rfAngle, const Vector3& rkAxis);
	void ToAngleAxis(float& rfAngle, Vector3& rkAxis) const;

	void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
	void ToAxes(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

	Vector3 getXAxis() const;
	Vector3 getYAxis() const;
	Vector3 getZAxis() const;

	inline Quaternion& operator=(const Quaternion& rkQ)
	{
		w = rkQ.w;
		x = rkQ.x;
		y = rkQ.y;
		z = rkQ.z;
		return *this;
	}

	Quaternion operator+(const Quaternion& other) const;
	Quaternion operator-(const Quaternion& other) const;
	Quaternion operator*(const Quaternion& other) const;
	Quaternion operator*(float fScalar) const;
	friend Quaternion operator*(float fScalar, const Quaternion& rkQ);
	Quaternion operator-() const;

	inline bool operator==(const Quaternion& rhs) const
	{
		return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
	}

	inline bool operator!=(const Quaternion& rhs) const
	{
		return !operator==(rhs);
	}

	// functions of a quaternion
	float Dot(const Quaternion& rkQ) const;  // dot product
	inline float LengthSquared() const { return w*w + x*x + y*y + z*z; }

	float Normalise(); 
	Quaternion getInverse() const;
	Quaternion getConjugate() const;
	Quaternion Exp() const;
	Quaternion Log() const;

	// rotation of a vector by a quaternion
	Vector3 operator*(const Vector3& rkVector) const;
	friend Vector3 operator*(const Vector3& rkVector, const Quaternion& rkQ);

	/** Calculate the local roll element of this quaternion.
	@param reprojectAxis By default the method returns the 'intuitive' result
		that is, if you projected the local Y of the quaternion onto the X and
		Y axes, the angle between them is returned. If set to false though, the
		result is the actual yaw that will be used to implement the quaternion,
		which is the shortest possible path to get to the same orientation and 
		may involve less axial rotation. 
	*/
	float getRoll(bool reprojectAxis = true) const;

	/** Calculate the local pitch element of this quaternion
	@param reprojectAxis By default the method returns the 'intuitive' result
		that is, if you projected the local Z of the quaternion onto the X and
		Y axes, the angle between them is returned. If set to true though, the
		result is the actual yaw that will be used to implement the quaternion,
		which is the shortest possible path to get to the same orientation and 
		may involve less axial rotation. 
	*/
	float getPitch(bool reprojectAxis = true) const;

	/** Calculate the local yaw element of this quaternion
	@param reprojectAxis By default the method returns the 'intuitive' result
		that is, if you projected the local Z of the quaternion onto the X and
		Z axes, the angle between them is returned. If set to true though, the
		result is the actual yaw that will be used to implement the quaternion,
		which is the shortest possible path to get to the same orientation and 
		may involve less axial rotation. 
	*/
	float getYaw(bool reprojectAxis = true) const;

	/// Equality with tolerance (tolerance is max angle difference)
	bool equals(const Quaternion& rhs, const float& tolerance) const;
		
	// spherical linear interpolation
	static Quaternion Slerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath = false);

	static Quaternion SlerpExtraSpins(float fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins);

	// setup for spherical quadratic interpolation
	static void Intermediate(const Quaternion& rkQ0, const Quaternion& rkQ1, const Quaternion& rkQ2, Quaternion& rka, Quaternion& rkB);

	// spherical quadratic interpolation
	static Quaternion Squad(float fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath = false);

	// normalised linear interpolation - faster but less accurate (non-constant rotation velocity)
	static Quaternion nlerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath = false);

	// Check whether this quaternion contains valid values
	inline bool isNaN() const
	{
		return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
	}

	// Function for writing to a stream. Outputs "Quaternion(w, x, y, z)" with w,x,y,z being the member values of the quaternion.
	inline friend std::ostream& operator<<(std::ostream& o, const Quaternion& q)
	{
		o << "Quaternion(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
		return o;
	}

	static const Quaternion Identity;
	static const Quaternion Zero;

public:

	float w, x, y, z;
};

//-----------------------------------//

#endif // __Quaternion_h__