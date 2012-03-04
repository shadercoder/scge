#ifndef __Vector2_h__
#define __Vector2_h__

#include "scge\Math.h"

//-----------------------------------//

/**
 * Simple struct to hold 2-dimensional stuff.
 */
template<typename T>
class Vector2T
{
public:
	T x, y;

	//-----------------------------------//
	
	// Default empty constructor
	Vector2T()
		: x(0), y(0)
	{ }

	//-----------------------------------//
	
	// Constructs a vector from 2 values
	Vector2T(const T x, const T y)
		: x(x), y(y)
	{ }

	// Constructs a vector from 1 value
	Vector2T(const T f)
		: x(f), y(f)
	{ }

	//-----------------------------------//
	
	// Copy constructor
	Vector2T (const Vector2T<T>& v)
		: x(v.x), y(v.y)
	{ }

	inline explicit Vector2T(const T afCoordinate[2])
		: x( afCoordinate[0] )
		, y( afCoordinate[1] )
	{
	}

	inline void swap(Vector2T<T>& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
	}

	inline T operator[](const size_t i) const
	{
		return *(&x+i);
	}

	inline T& operator[](const size_t i)
	{
		return *(&x+i);
	}

	// Pointer accessor for direct copying
	inline T* ptr()
	{
		return &x;
	}
	// Pointer accessor for direct copying
	inline const T* ptr() const
	{
		return &x;
	}

	/** Assigns the value of the other vector.
		@param
			rkVector The other vector
	*/
	inline Vector2T<T>& operator=(const Vector2T<T>& rkVector)
	{
		x = rkVector.x;
		y = rkVector.y;

		return *this;
	}

	inline Vector2T<T>& operator=(const T fScalar)
	{
		x = fScalar;
		y = fScalar;

		return *this;
	}

	inline bool operator==(const Vector2T<T>& rkVector) const
	{
		return ( x == rkVector.x && y == rkVector.y );
	}

	inline bool operator!=( const Vector2T<T>& rkVector) const
	{
		return ( x != rkVector.x || y != rkVector.y  );
	}

	// arithmetic operations
	inline Vector2T<T> operator+( const Vector2T<T>& rkVector ) const
	{
		return Vector2T<T>(
			x + rkVector.x,
			y + rkVector.y);
	}

	inline Vector2T<T> operator-( const Vector2T<T>& rkVector ) const
	{
		return Vector2T<T>(
			x - rkVector.x,
			y - rkVector.y);
	}

	inline Vector2T<T> operator*( const T fScalar ) const
	{
		return Vector2T<T>(
			x * fScalar,
			y * fScalar);
	}

	inline Vector2T<T> operator*( const Vector2T<T>& rhs) const
	{
		return Vector2T<T>(
			x * rhs.x,
			y * rhs.y);
	}

	inline Vector2T<T> operator/( const T fScalar ) const
	{
		T fInv = 1.0f / fScalar;

		return Vector2T<T>(
			x * fInv,
			y * fInv);
	}

	inline Vector2T<T> operator/( const Vector2T<T>& rhs) const
	{
		return Vector2T<T>(
			x / rhs.x,
			y / rhs.y);
	}

	inline const Vector2T<T>& operator+() const
	{
		return *this;
	}

	inline Vector2T<T> operator-() const
	{
		return Vector2T<T>(-x, -y);
	}

	template <typename T>
	inline friend Vector2T<T> operator*( const T fScalar, const Vector2T<T>& rkVector )
	{
		return Vector2T<T>(
			fScalar * rkVector.x,
			fScalar * rkVector.y);
	}

	template <typename T>
	inline friend Vector2T<T> operator/( const T fScalar, const Vector2T<T>& rkVector )
	{
		return Vector2T<T>(
			fScalar / rkVector.x,
			fScalar / rkVector.y);
	}

	template <typename T>
	inline friend Vector2T<T> operator+(const Vector2T<T>& lhs, const T rhs)
	{
		return Vector2T<T>(
			lhs.x + rhs,
			lhs.y + rhs);
	}

	template <typename T>
	inline friend Vector2T<T> operator+(const T lhs, const Vector2T<T>& rhs)
	{
		return Vector2T<T>(
			lhs + rhs.x,
			lhs + rhs.y);
	}

	template <typename T>
	inline friend Vector2T<T> operator-(const Vector2T<T>& lhs, const T rhs)
	{
		return Vector2T<T>(
			lhs.x - rhs,
			lhs.y - rhs);
	}

	template <typename T>
	inline friend Vector2T<T> operator-(const T lhs, const Vector2T<T>& rhs)
	{
		return Vector2T<T>(
			lhs - rhs.x,
			lhs - rhs.y);
	}

	// arithmetic updates
	inline Vector2T<T>& operator+=(const Vector2T<T>& rkVector)
	{
		x += rkVector.x;
		y += rkVector.y;

		return *this;
	}

	inline Vector2T<T>& operator += ( const T fScaler )
	{
		x += fScaler;
		y += fScaler;

		return *this;
	}

	inline Vector2T<T>& operator -= ( const Vector2T<T>& rkVector )
	{
		x -= rkVector.x;
		y -= rkVector.y;

		return *this;
	}

	inline Vector2T<T>& operator -= ( const T fScaler )
	{
		x -= fScaler;
		y -= fScaler;

		return *this;
	}

	inline Vector2T<T>& operator *= ( const T fScalar )
	{
		x *= fScalar;
		y *= fScalar;

		return *this;
	}

	inline Vector2T<T>& operator *= ( const Vector2T<T>& rkVector )
	{
		x *= rkVector.x;
		y *= rkVector.y;

		return *this;
	}

	inline Vector2T<T>& operator /= ( const T fScalar )
	{
		T fInv = 1.0f / fScalar;

		x *= fInv;
		y *= fInv;

		return *this;
	}

	inline Vector2T<T>& operator /= ( const Vector2T<T>& rkVector )
	{
		x /= rkVector.x;
		y /= rkVector.y;

		return *this;
	}

	/** Returns the length (magnitude) of the vector.
		@warning
			This operation requires a square root and is expensive in
			terms of CPU operations. If you don't need to know the exact
			length (e.g. for just comparing lengths) use squaredLength()
			instead.
	*/
	inline T length () const
	{
		return Math::Sqrt( x * x + y * y );
	}

	/** Returns the square of the length(magnitude) of the vector.
		@remarks
			This  method is for efficiency - calculating the actual
			length of a vector requires a square root, which is expensive
			in terms of the operations required. This method returns the
			square of the length of the vector, i.e. the same as the
			length but before the square root is taken. Use this if you
			want to find the longest / shortest vector without incurring
			the square root.
	*/
	inline T squaredLength () const
	{
		return x * x + y * y;
	}
	/** Returns the distance to another vector.
		@warning
			This operation requires a square root and is expensive in
			terms of CPU operations. If you don't need to know the exact
			distance (e.g. for just comparing distances) use squaredDistance()
			instead.
	*/
	inline T distance(const Vector2T<T>& rhs) const
	{
		return (*this - rhs).length();
	}

	/** Returns the square of the distance to another vector.
		@remarks
			This method is for efficiency - calculating the actual
			distance to another vector requires a square root, which is
			expensive in terms of the operations required. This method
			returns the square of the distance to another vector, i.e.
			the same as the distance but before the square root is taken.
			Use this if you want to find the longest / shortest distance
			without incurring the square root.
	*/
	inline T squaredDistance(const Vector2T<T>& rhs) const
	{
		return (*this - rhs).squaredLength();
	}

	/** Calculates the dot (scalar) product of this vector with another.
		@remarks
			The dot product can be used to calculate the angle between 2
			vectors. If both are unit vectors, the dot product is the
			cosine of the angle; otherwise the dot product must be
			divided by the product of the lengths of both vectors to get
			the cosine of the angle. This result can further be used to
			calculate the distance of a point from a plane.
		@param
			vec Vector with which to calculate the dot product (together
			with this one).
		@returns
			A float representing the dot product value.
	*/
	inline T dotProduct(const Vector2T<T>& vec) const
	{
		return x * vec.x + y * vec.y;
	}

	/** Normalises the vector.
		@remarks
			This method normalises the vector such that it's
			length / magnitude is 1. The result is called a unit vector.
		@note
			This function will not crash for zero-sized vectors, but there
			will be no changes made to their components.
		@returns The previous length of the vector.
	*/
	inline T normalise()
	{
		T fLength = Math::Sqrt( x * x + y * y);

		// Will also work for zero-sized vectors, but will change nothing
		if ( fLength > Limits::FloatEpsilon )
		{
			T fInvLength = 1.0f / fLength;
			x *= fInvLength;
			y *= fInvLength;
		}

		return fLength;
	}



	/** Returns a vector at a point half way between this and the passed
		in vector.
	*/
	inline Vector2T<T> midPoint( const Vector2T<T>& vec ) const
	{
		return Vector2T<T>(
			( x + vec.x ) * 0.5f,
			( y + vec.y ) * 0.5f );
	}

	/** Returns true if the vector's scalar components are all greater
		that the ones of the vector it is compared against.
	*/
	inline bool operator < ( const Vector2T<T>& rhs ) const
	{
		if( x < rhs.x && y < rhs.y )
			return true;
		return false;
	}

	/** Returns true if the vector's scalar components are all smaller
		that the ones of the vector it is compared against.
	*/
	inline bool operator > ( const Vector2T<T>& rhs ) const
	{
		if( x > rhs.x && y > rhs.y )
			return true;
		return false;
	}

	/** Sets this vector's components to the minimum of its own and the
		ones of the passed in vector.
		@remarks
			'Minimum' in this case means the combination of the lowest
			value of x, y and z from both vectors. Lowest is taken just
			numerically, not magnitude, so -1 < 0.
	*/
	inline void makeFloor( const Vector2T<T>& cmp )
	{
		if( cmp.x < x ) x = cmp.x;
		if( cmp.y < y ) y = cmp.y;
	}

	/** Sets this vector's components to the maximum of its own and the
		ones of the passed in vector.
		@remarks
			'Maximum' in this case means the combination of the highest
			value of x, y and z from both vectors. Highest is taken just
			numerically, not magnitude, so 1 > -3.
	*/
	inline void makeCeil( const Vector2T<T>& cmp )
	{
		if( cmp.x > x ) x = cmp.x;
		if( cmp.y > y ) y = cmp.y;
	}

	/** Generates a vector perpendicular to this vector (eg an 'up' vector).
		@remarks
			This method will return a vector which is perpendicular to this
			vector. There are an infinite number of possibilities but this
			method will guarantee to generate one of them. If you need more
			control you should use the Quaternion class.
	*/
	inline Vector2T<T> perpendicular(void) const
	{
		return Vector2T<T> (-y, x);
	}
	/** Calculates the 2 dimensional cross-product of 2 vectors, which results
		in a single floating point value which is 2 times the area of the triangle.
	*/
	inline T crossProduct( const Vector2T<T>& rkVector ) const
	{
		return x * rkVector.y - y * rkVector.x;
	}
	/** Generates a new random vector which deviates from this vector by a
		given angle in a random direction.
		@remarks
			This method assumes that the random number generator has already
			been seeded appropriately.
		@param
			angle The angle at which to deviate in radians
		@param
			up Any vector perpendicular to this one (which could generated
			by cross-product of this vector and any other non-colinear
			vector). If you choose not to provide this the function will
			derive one on it's own, however if you provide one yourself the
			function will be faster (this allows you to reuse up vectors if
			you call this method more than once)
		@returns
			A random vector which deviates from this vector by angle. This
			vector will not be normalised, normalise it if you wish
			afterwards.
	*/
	inline Vector2T<T> randomDeviant(T angle) const
	{

		angle *=  Math::UnitRandom() * Math::TwoPi;
		T cosa = cos(angle);
		T sina = sin(angle);
		return  Vector2T<T>(cosa * x - sina * y, sina * x + cosa * y);
	}

	/** Returns true if this vector is zero length. */
	inline bool isZeroLength(void) const
	{
		T sqlen = (x * x) + (y * y);
		return (sqlen < (1e-06 * 1e-06));

	}

	/** As normalise, except that this vector is unaffected and the
		normalised vector is returned as a copy. */
	inline Vector2T<T> normalisedCopy(void) const
	{
		Vector2T<T> ret = *this;
		ret.normalise();
		return ret;
	}

	/** Calculates a reflection vector to the plane with the given normal .
	@remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
	*/
	inline Vector2T<T> reflect(const Vector2T<T>& normal) const
	{
		return Vector2T<T>( *this - ( 2 * this->dotProduct(normal) * normal ) );
	}
	/// Check whether this vector contains valid values
	inline bool isNaN() const
	{
		return Math::isNaN(x) || Math::isNaN(y);
	}

	// special points
	static const Vector2T<float> Zero;
	static const Vector2T<float> UnitX;
	static const Vector2T<float> UnitY;
	static const Vector2T<float> NegativeUnitX;
	static const Vector2T<float> NegativeUnitY;
	static const Vector2T<float> UnitScale;

	/** Function for writing to a stream.
	*/
	inline friend std::ostream& operator<<(std::ostream& o, const Vector2T<T>& v)
	{
		o << "Vector2(" << v.x << ", " << v.y <<  ")";
		return o;
	}
};

//-----------------------------------//

typedef Vector2T<float> Vector2;
typedef Vector2T<int> Vector2i;

//-----------------------------------//

#endif // __Vector2_h__