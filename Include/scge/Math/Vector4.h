#ifndef __Vector4_h__
#define __Vector4_h__

#include "scge\Math.h"

//-----------------------------------//

/**
 * Represents a 4-dimensional vector class template.
 */

template <typename T>
class Vector4T
{
public:
	T x, y, z, w;

	// Default empty constructor.
	Vector4T ()
		: x(), y(), z(), w()
	{ }

	//-----------------------------------//
	
	// Copy constructor.
	Vector4T (const Vector4T<T>& v)
		: x(v.x), y(v.y), z(v.z), w(v.w)
	{ }

	//-----------------------------------//
	
	// Constructs a vector from a vector and 1 value.
	Vector4T (const Vector3T<T>& v, T w = T(1))
		: x(v.x), y(v.y), z(v.z), w(w)
	{ }

	//-----------------------------------//

	// Constructs a vector from 1 value.
	Vector4T (const T v)
		: x(v), y(v), z(v), w(v)
	{ }

	//-----------------------------------//

	// Constructs a vector from 4 values.
	Vector4T (const T a, const T b, const T c, const T d)
		: x(a), y(b), z(c), w(d)
	{ }

	inline explicit Vector4T( const T afCoordinate[4] )
		: x( afCoordinate[0] )
		, y( afCoordinate[1] )
		, z( afCoordinate[2] )
		, w( afCoordinate[3] )
	{
	}

	inline explicit Vector4T( T* const r )
		: x( r[0] ), y( r[1] ), z( r[2] ), w( r[3] )
	{
	}

	inline void swap(Vector4T<T>& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
		std::swap(w, other.w);
	}

	inline T operator [] ( const size_t i ) const
	{
		//assert( i < 4 );

		return *(&x+i);
	}

	inline T& operator [] ( const size_t i )
	{
		//assert( i < 4 );

		return *(&x+i);
	}

	/// Pointer accessor for direct copying
	inline T* ptr()
	{
		return &x;
	}
	/// Pointer accessor for direct copying
	inline const T* ptr() const
	{
		return &x;
	}

	/** Assigns the value of the other vector.
		@param
			rkVector The other vector
	*/
	inline Vector4T<T>& operator=(const Vector4T<T>& rkVector)
	{
		x = rkVector.x;
		y = rkVector.y;
		z = rkVector.z;
		w = rkVector.w;

		return *this;
	}

	inline Vector4T<T>& operator=(const T fScalar)
	{
		x = fScalar;
		y = fScalar;
		z = fScalar;
		w = fScalar;
		return *this;
	}

	inline bool operator==(const Vector4T<T>& rkVector) const
	{
		return ( x == rkVector.x &&
			y == rkVector.y &&
			z == rkVector.z &&
			w == rkVector.w );
	}

	inline bool operator!=(const Vector4T<T>& rkVector) const
	{
		return ( x != rkVector.x ||
			y != rkVector.y ||
			z != rkVector.z ||
			w != rkVector.w );
	}

	inline Vector4T<T>& operator=(const Vector3T<T>& rhs)
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		w = 1.0f;
		return *this;
	}

	// arithmetic operations
	inline Vector4T<T> operator+( const Vector4T<T>& rkVector) const
	{
		return Vector4T<T>(
			x + rkVector.x,
			y + rkVector.y,
			z + rkVector.z,
			w + rkVector.w);
	}

	inline Vector4T<T> operator-(const Vector4T<T>& rkVector) const
	{
		return Vector4T<T>(
			x - rkVector.x,
			y - rkVector.y,
			z - rkVector.z,
			w - rkVector.w);
	}

	inline Vector4T<T> operator*(const T fScalar) const
	{
		return Vector4T<T>(
			x * fScalar,
			y * fScalar,
			z * fScalar,
			w * fScalar);
	}

	inline Vector4T<T> operator*(const Vector4T<T>& rhs) const
	{
		return Vector4T<T>(
			rhs.x * x,
			rhs.y * y,
			rhs.z * z,
			rhs.w * w);
	}

	inline Vector4T<T> operator/(const T fScalar) const
	{
		//assert( fScalar != 0.0 );

		T fInv = 1.0f / fScalar;

		return Vector4T<T>(
			x * fInv,
			y * fInv,
			z * fInv,
			w * fInv);
	}

	inline Vector4T<T> operator/( const Vector4T<T>& rhs) const
	{
		return Vector4T<T>(
			x / rhs.x,
			y / rhs.y,
			z / rhs.z,
			w / rhs.w);
	}

	inline const Vector4T<T>& operator+() const
	{
		return *this;
	}

	inline Vector4T<T> operator-() const
	{
		return Vector4T<T>(-x, -y, -z, -w);
	}

	inline friend Vector4T<T> operator*(const T fScalar, const Vector4T<T>& rkVector)
	{
		return Vector4T<T>(
			fScalar * rkVector.x,
			fScalar * rkVector.y,
			fScalar * rkVector.z,
			fScalar * rkVector.w);
	}

	inline friend Vector4T<T> operator/(const T fScalar, const Vector4T<T>& rkVector)
	{
		return Vector4T<T>(
			fScalar / rkVector.x,
			fScalar / rkVector.y,
			fScalar / rkVector.z,
			fScalar / rkVector.w);
	}

	inline friend Vector4T<T> operator+(const Vector4T<T>& lhs, const T rhs)
	{
		return Vector4T<T>(
			lhs.x + rhs,
			lhs.y + rhs,
			lhs.z + rhs,
			lhs.w + rhs);
	}

	inline friend Vector4T<T> operator+(const T lhs, const Vector4T<T>& rhs)
	{
		return Vector4T<T>(
			lhs + rhs.x,
			lhs + rhs.y,
			lhs + rhs.z,
			lhs + rhs.w);
	}

	inline friend Vector4T<T> operator-(const Vector4T<T>& lhs, T rhs)
	{
		return Vector4T<T>(
			lhs.x - rhs,
			lhs.y - rhs,
			lhs.z - rhs,
			lhs.w - rhs);
	}

	inline friend Vector4T<T> operator-(const T lhs, const Vector4T<T>& rhs)
	{
		return Vector4T<T>(
			lhs - rhs.x,
			lhs - rhs.y,
			lhs - rhs.z,
			lhs - rhs.w);
	}

	// arithmetic updates
	inline Vector4T<T>& operator+=(const Vector4T<T>& rkVector)
	{
		x += rkVector.x;
		y += rkVector.y;
		z += rkVector.z;
		w += rkVector.w;

		return *this;
	}

	inline Vector4T<T>& operator-=(const Vector4T<T>& rkVector)
	{
		x -= rkVector.x;
		y -= rkVector.y;
		z -= rkVector.z;
		w -= rkVector.w;

		return *this;
	}

	inline Vector4T<T>& operator*=(const T fScalar)
	{
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;
		w *= fScalar;
		return *this;
	}

	inline Vector4T<T>& operator+=(const T fScalar)
	{
		x += fScalar;
		y += fScalar;
		z += fScalar;
		w += fScalar;
		return *this;
	}

	inline Vector4T<T>& operator-=(const T fScalar)
	{
		x -= fScalar;
		y -= fScalar;
		z -= fScalar;
		w -= fScalar;
		return *this;
	}

	inline Vector4T<T>& operator*=(const Vector4T<T>& rkVector)
	{
		x *= rkVector.x;
		y *= rkVector.y;
		z *= rkVector.z;
		w *= rkVector.w;

		return *this;
	}

	inline Vector4T<T>& operator/=(const T fScalar)
	{
		//assert( fScalar != 0.0 );

		T fInv = 1.0f / fScalar;

		x *= fInv;
		y *= fInv;
		z *= fInv;
		w *= fInv;

		return *this;
	}

	inline Vector4T<T>& operator/=( const Vector4T<T>& rkVector)
	{
		x /= rkVector.x;
		y /= rkVector.y;
		z /= rkVector.z;
		w /= rkVector.w;

		return *this;
	}

	/** Calculates the dot (scalar) product of this vector with another.
		@param
			vec Vector with which to calculate the dot product (together
			with this one).
		@returns
			A float representing the dot product value.
	*/
	inline T dotProduct(const Vector4T<T>& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
	}
	/// Check whether this vector contains valid values
	inline bool isNaN() const
	{
		return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
	}
	/** Function for writing to a stream.
	*/
	inline friend std::ostream& operator<<(std::ostream& o, const Vector4T<T>& v)
	{
		o << "Vector4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
		return o;
	}

	//-----------------------------------//

	//-----------------------------------//

	// Special points
	static const Vector4T<float> Zero;
	static const Vector4T<float> UnitX;
	static const Vector4T<float> UnitY;
	static const Vector4T<float> UnitZ;
	static const Vector4T<float> UnitW;
};

//-----------------------------------//

/**
 * Specific types
 */

typedef Vector4T<int> Vector4i;
typedef Vector4T<float> Vector4;
typedef Vector4T<double> Vector4d;

//-----------------------------------//



#endif // __Vector4_h__
