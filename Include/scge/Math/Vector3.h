#ifndef __Vector3_h__
#define __Vector3_h__

#include "scge\Math.h"

#include "scge\Math\Quaternion.h"

//-----------------------------------//

template <typename T>
class Vector3T
{
public:
	T x, y, z;

	// Default empty constructor.
	inline Vector3T() : x(), y(), z() {}

	//-----------------------------------//
	
	// Copy constructor.
	Vector3T(const Vector3T<T>& v) : x(v.x), y(v.y), z(v.z) {}

	//-----------------------------------//

	// Constructs a vector from 1 value.
	Vector3T(const T v) : x(v), y(v), z(v) {}

	//-----------------------------------//

	// Constructs a vector from 3 values.
	Vector3T(const T a, const T b, const T c) : x(a), y(b), z(c) {}

	inline explicit Vector3T(const T afCoordinate[3])
		: x( afCoordinate[0] )
		, y( afCoordinate[1] )
		, z( afCoordinate[2] )
	{
	}

	inline explicit Vector3T(T* const r)
		: x( r[0] ), y( r[1] ), z( r[2] )
	{
	}

	inline void swap(Vector3T<T>& other)
	{
		std::swap(x, other.x);
		std::swap(y, other.y);
		std::swap(z, other.z);
	}

	inline T operator[](const size_t i) const
	{
		return *(&x + i);
	}

	inline T& operator[](const size_t i)
	{
		return *(&x + i);
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
	inline Vector3T<T>& operator=(const Vector3T<T>& rkVector)
	{
		x = rkVector.x;
		y = rkVector.y;
		z = rkVector.z;

		return *this;
	}

	inline Vector3T<T>& operator=(const T fScaler)
	{
		x = fScaler;
		y = fScaler;
		z = fScaler;

		return *this;
	}

	inline bool operator==(const Vector3T<T>& rkVector) const
	{
		return ( x == rkVector.x && y == rkVector.y && z == rkVector.z );
	}

	inline bool operator!=(const Vector3T<T>& rkVector) const
	{
		return ( x != rkVector.x || y != rkVector.y || z != rkVector.z );
	}

	// arithmetic operations
	inline Vector3T<T> operator+(const Vector3T<T>& rkVector) const
	{
		return Vector3T<T>(
			x + rkVector.x,
			y + rkVector.y,
			z + rkVector.z);
	}

	inline Vector3T<T> operator-(const Vector3T<T>& rkVector) const
	{
		return Vector3T<T>(
			x - rkVector.x,
			y - rkVector.y,
			z - rkVector.z);
	}

	inline Vector3T<T> operator*(const T fScalar) const
	{
		return Vector3T<T>(
			x * fScalar,
			y * fScalar,
			z * fScalar);
	}

	inline Vector3T<T> operator*(const Vector3T<T>& rhs) const
	{
		return Vector3T<T>(
			x * rhs.x,
			y * rhs.y,
			z * rhs.z);
	}

	inline Vector3T<T> operator/(const T fScalar) const
	{
	   T fInv = 1.0f / fScalar;

		return Vector3T<T>(
			x * fInv,
			y * fInv,
			z * fInv);
	}

	inline Vector3T<T> operator/(const Vector3T<T>& rhs) const
	{
		return Vector3T<T>(
			x / rhs.x,
			y / rhs.y,
			z / rhs.z);
	}

	inline const Vector3T<T>& operator+() const
	{
		return *this;
	}

	inline Vector3T<T> operator-() const
	{
		return Vector3T<T>(-x, -y, -z);
	}

	inline friend Vector3T<T> operator*(const T fScalar, const Vector3T<T>& rkVector)
	{
		return Vector3T<T>(
			fScalar * rkVector.x,
			fScalar * rkVector.y,
			fScalar * rkVector.z);
	}

	inline friend Vector3T<T> operator/(const T fScalar, const Vector3T<T>& rkVector)
	{
		return Vector3T<T>(
			fScalar / rkVector.x,
			fScalar / rkVector.y,
			fScalar / rkVector.z);
	}

	inline friend Vector3T<T> operator+(const Vector3T<T>& lhs, const T rhs)
	{
		return Vector3T<T>(
			lhs.x + rhs,
			lhs.y + rhs,
			lhs.z + rhs);
	}

	inline friend Vector3T<T> operator+(const T lhs, const Vector3T<T>& rhs)
	{
		return Vector3T<T>(
			lhs + rhs.x,
			lhs + rhs.y,
			lhs + rhs.z);
	}

	inline friend Vector3T<T> operator-(const Vector3T<T>& lhs, const T rhs)
	{
		return Vector3T<T>(
			lhs.x - rhs,
			lhs.y - rhs,
			lhs.z - rhs);
	}

	inline friend Vector3T<T> operator-(const T lhs, const Vector3T<T>& rhs)
	{
		return Vector3T<T>(
			lhs - rhs.x,
			lhs - rhs.y,
			lhs - rhs.z);
	}

	// arithmetic updates
	inline Vector3T<T>& operator+=(const Vector3T<T>& rkVector)
	{
		x += rkVector.x;
		y += rkVector.y;
		z += rkVector.z;

		return *this;
	}

	inline Vector3T<T>& operator+=(const T fScalar)
	{
		x += fScalar;
		y += fScalar;
		z += fScalar;
		return *this;
	}

	inline Vector3T<T>& operator-=(const Vector3T<T>& rkVector)
	{
		x -= rkVector.x;
		y -= rkVector.y;
		z -= rkVector.z;

		return *this;
	}

	inline Vector3T<T>& operator-=(const T fScalar)
	{
		x -= fScalar;
		y -= fScalar;
		z -= fScalar;
		return *this;
	}

	inline Vector3T<T>& operator*=(const T fScalar)
	{
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;
		return *this;
	}

	inline Vector3T<T>& operator*=(const Vector3T<T>& rkVector)
	{
		x *= rkVector.x;
		y *= rkVector.y;
		z *= rkVector.z;

		return *this;
	}

	inline Vector3T<T>& operator/=(const T fScalar)
	{
		T fInv = 1.0f / fScalar;

		x *= fInv;
		y *= fInv;
		z *= fInv;

		return *this;
	}

	inline Vector3T<T>& operator/=(const Vector3T<T>& rkVector)
	{
		x /= rkVector.x;
		y /= rkVector.y;
		z /= rkVector.z;

		return *this;
	}


	/** Returns the length (magnitude) of the vector.
		@warning
			This operation requires a square root and is expensive in
			terms of CPU operations. If you don't need to know the exact
			length (e.g. for just comparing lengths) use squaredLength()
			instead.
	*/
	inline T length() const
	{
		return Math::Sqrt( x * x + y * y + z * z );
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
	inline T squaredLength() const
	{
		return x * x + y * y + z * z;
	}

	/** Returns the distance to another vector.
		@warning
			This operation requires a square root and is expensive in
			terms of CPU operations. If you don't need to know the exact
			distance (e.g. for just comparing distances) use squaredDistance()
			instead.
	*/
	inline T distance(const Vector3T<T>& rhs) const
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
	inline T squaredDistance(const Vector3T<T>& rhs) const
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
	inline T dotProduct(const Vector3T<T>& vec) const
	{
		return x * vec.x + y * vec.y + z * vec.z;
	}

	/** Calculates the absolute dot (scalar) product of this vector with another.
		@remarks
			This function work similar dotProduct, except it use absolute value
			of each component of the vector to computing.
		@param
			vec Vector with which to calculate the absolute dot product (together
			with this one).
		@returns
			A Real representing the absolute dot product value.
	*/
	inline T absDotProduct(const Vector3T<T>& vec) const
	{
		return Math::Abs(x * vec.x) + Math::Abs(y * vec.y) + Math::Abs(z * vec.z);
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
	inline T Normalise()
	{
		T fLength = Math::Sqrt( x * x + y * y + z * z );

		if(fLength > Math::Limits::FloatEpsilon)
		{
			T fInvLength = 1.0f / fLength;
			x *= fInvLength;
			y *= fInvLength;
			z *= fInvLength;
		}

		return fLength;
	}

	/** Calculates the cross-product of 2 vectors, i.e. the vector that
		lies perpendicular to them both.
		@remarks
			The cross-product is normally used to calculate the normal
			vector of a plane, by calculating the cross-product of 2
			non-equivalent vectors which lie on the plane (e.g. 2 edges
			of a triangle).
		@param
			vec Vector which, together with this one, will be used to
			calculate the cross-product.
		@returns
			A vector which is the result of the cross-product. This
			vector will <b>NOT</b> be normalised, to maximise efficiency
			- call Vector3::normalise on the result if you wish this to
			be done. As for which side the resultant vector will be on, the
			returned vector will be on the side from which the arc from 'this'
			to rkVector is anticlockwise, e.g. UNIT_Y.crossProduct(UNIT_Z)
			= UNIT_X, whilst UNIT_Z.crossProduct(UNIT_Y) = -UNIT_X.
			This is because OGRE uses a right-handed coordinate system.
		@par
			For a clearer explanation, look a the left and the bottom edges
			of your monitor's screen. Assume that the first vector is the
			left edge and the second vector is the bottom edge, both of
			them starting from the lower-left corner of the screen. The
			resulting vector is going to be perpendicular to both of them
			and will go <i>inside</i> the screen, towards the cathode tube
			(assuming you're using a CRT monitor, of course).
	*/
	inline Vector3T<T> crossProduct(const Vector3T<T>& rkVector) const
	{
		return Vector3T<T>(
			y * rkVector.z - z * rkVector.y,
			z * rkVector.x - x * rkVector.z,
			x * rkVector.y - y * rkVector.x);
	}

	/** Returns a vector at a point half way between this and the passed
		in vector.
	*/
	inline Vector3T<T> midPoint(const Vector3T<T>& vec) const
	{
		return Vector3T<T>(
			( x + vec.x ) * 0.5f,
			( y + vec.y ) * 0.5f,
			( z + vec.z ) * 0.5f );
	}

	/** Returns true if the vector's scalar components are all greater
		that the ones of the vector it is compared against.
	*/
	inline bool operator<(const Vector3T<T>& rhs) const
	{
		if( x < rhs.x && y < rhs.y && z < rhs.z )
			return true;
		return false;
	}

	/** Returns true if the vector's scalar components are all smaller
		that the ones of the vector it is compared against.
	*/
	inline bool operator>(const Vector3T<T>& rhs) const
	{
		if( x > rhs.x && y > rhs.y && z > rhs.z )
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
	inline void makeFloor(const Vector3T<T>& cmp)
	{
		if( cmp.x < x ) x = cmp.x;
		if( cmp.y < y ) y = cmp.y;
		if( cmp.z < z ) z = cmp.z;
	}

	/** Sets this vector's components to the maximum of its own and the
		ones of the passed in vector.
		@remarks
			'Maximum' in this case means the combination of the highest
			value of x, y and z from both vectors. Highest is taken just
			numerically, not magnitude, so 1 > -3.
	*/
	inline void makeCeil(const Vector3T<T>& cmp)
	{
		if( cmp.x > x ) x = cmp.x;
		if( cmp.y > y ) y = cmp.y;
		if( cmp.z > z ) z = cmp.z;
	}

	/** Generates a vector perpendicular to this vector (eg an 'up' vector).
		@remarks
			This method will return a vector which is perpendicular to this
			vector. There are an infinite number of possibilities but this
			method will guarantee to generate one of them. If you need more
			control you should use the Quaternion class.
	*/
	inline Vector3T<T> perpendicular() const
	{
		static const T fSquareZero = (T)(1e-06 * 1e-06);

		Vector3T<T> perp = this->crossProduct(Vector3T<T>(1, 0, 0));

		// Check length
		if( perp.squaredLength() < fSquareZero )
		{
			/* This vector is the Y axis multiplied by a scalar, so we have
				to use another axis.
			*/
			perp = this->crossProduct( Vector3T<T>(0, 1, 0) );
		}
		perp.Normalise();

		return perp;
	}
	/** Generates a new random vector which deviates from this vector by a
		given angle in a random direction.
		@remarks
			This method assumes that the random number generator has already
			been seeded appropriately.
		@param
			angle The angle at which to deviate
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
	inline Vector3T<T> randomDeviant(const float& angle, const Vector3T<T>& up = Vector3T<T>()) const
	{
		Vector3T<T> newUp;

		if (up == Vector3T<T>())
		{
			// Generate an up vector
			newUp = this->perpendicular();
		}
		else
		{
			newUp = up;
		}

		// Rotate up vector by random amount around this
		Quaternion q;
		q.FromAngleAxis(Math::UnitRandom() * Math::Constants::TwoPi, *this);
		newUp = q * newUp;

		// Finally rotate this by given angle around randomised up
		q.FromAngleAxis(angle, newUp);
		return q * (*this);
	}

	/** Gets the angle between 2 vectors.
	@remarks
		Vectors do not have to be unit-length but must represent directions.
	*/
	inline float angleBetween(const Vector3T<T>& dest)
	{
		T lenProduct = length() * dest.length();

		// Divide by zero check
		if(lenProduct < 1e-6f)
			lenProduct = 1e-6f;

		T f = dotProduct(dest) / lenProduct;

		f = Math::clamp(f, -1.0, 1.0);

		return Math::ACos(f);

	}

	/** Gets the shortest arc quaternion to rotate this vector to the destination
		vector.
	@remarks
		If you call this with a dest vector that is close to the inverse
		of this vector, we will rotate 180 degrees around the 'fallbackAxis'
		(if specified, or a generated axis if not) since in this case
		ANY axis of rotation is valid.
	*/
	Quaternion getRotationTo(const Vector3T<T>& dest, const Vector3T<T>& fallbackAxis = Vector3T<T>()) const
	{
		// Based on Stan Melax's article in Game Programming Gems
		Quaternion q;
		// Copy, since cannot modify local
		Vector3T<T> v0 = *this;
		Vector3T<T> v1 = dest;
		v0.Normalise();
		v1.Normalise();

		T d = v0.dotProduct(v1);
		// If dot == 1, vectors are the same
		if (d >= 1.0f)
		{
			return Quaternion::Identity;
		}
		if (d < (1e-6f - 1.0f))
		{
			if (fallbackAxis != Vector3T<T>())
			{
				// rotate 180 degrees about the fallback axis
				q.FromAngleAxis(Math::Constants::Pi, fallbackAxis);
			}
			else
			{
				// Generate an axis
				Vector3T<T> axis = UnitX.crossProduct(*this);
				if (axis.isZeroLength()) // pick another if colinear
					axis = UnitY.crossProduct(*this);
				axis.Normalise();
				q.FromAngleAxis(Math::Constants::Pi, axis);
			}
		}
		else
		{
			T s = Math::Sqrt( (1+d)*2 );
			T invs = 1 / s;

			Vector3T<T> c = v0.crossProduct(v1);

			q.x = c.x * invs;
			q.y = c.y * invs;
			q.z = c.z * invs;
			q.w = s * 0.5f;
			q.Normalise();
		}
		return q;
	}

	/** Returns true if this vector is zero length. */
	inline bool isZeroLength() const
	{
		T sqlen = (x * x) + (y * y) + (z * z);
		return (sqlen < (1e-06 * 1e-06));

	}

	/** As normalise, except that this vector is unaffected and the
		normalised vector is returned as a copy. */
	inline Vector3T<T> CreateNormalised() const
	{
		Vector3T<T> ret = *this;
		ret.Normalise();
		return ret;
	}

	/** Calculates a reflection vector to the plane with the given normal .
	@remarks NB assumes 'this' is pointing AWAY FROM the plane, invert if it is not.
	*/
	inline Vector3T<T> reflect(const Vector3T<T>& normal) const
	{
		return Vector3T<T>( *this - ( 2 * this->dotProduct(normal) * normal ) );
	}

	/** Returns whether this vector is within a positional tolerance
		of another vector.
	@param rhs The vector to compare with
	@param tolerance The amount that each element of the vector may vary by
		and still be considered equal
	*/
	inline bool positionEquals(const Vector3T<T>& rhs, T tolerance = 1e-03) const
	{
		return Math::Equal(x, rhs.x, tolerance) &&
			Math::Equal(y, rhs.y, tolerance) &&
			Math::Equal(z, rhs.z, tolerance);

	}

	/** Returns whether this vector is within a positional tolerance
		of another vector, also take scale of the vectors into account.
	@param rhs The vector to compare with
	@param tolerance The amount (related to the scale of vectors) that distance
		of the vector may vary by and still be considered close
	*/
	inline bool positionCloses(const Vector3T<T>& rhs, T tolerance = 1e-03f) const
	{
		return squaredDistance(rhs) <= (squaredLength() + rhs.squaredLength()) * tolerance;
	}

	/** Returns whether this vector is within a directional tolerance
		of another vector.
	@param rhs The vector to compare with
	@param tolerance The maximum angle by which the vectors may vary and
		still be considered equal
	@note Both vectors should be normalised.
	*/
	inline bool directionEquals(const Vector3T<T>& rhs, const T& tolerance) const
	{
		T dot = dotProduct(rhs);
		float angle = Math::ACos(dot);

		return Math::Abs(angle) <= tolerance;

	}

	/// Check whether this vector contains valid values
	inline bool isNaN() const
	{
		return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z);
	}

	// special points
	static const Vector3T<float> Zero;
	static const Vector3T<float> UnitX;
	static const Vector3T<float> UnitY;
	static const Vector3T<float> UnitZ;
	static const Vector3T<float> NegativeUnitX;
	static const Vector3T<float> NegativeUnitY;
	static const Vector3T<float> NegativeUnitZ;
	static const Vector3T<float> UnitScale;

	/** Function for writing to a stream.
	*/
	inline friend std::ostream& operator<<(std::ostream& o, const Vector3T<T>& v)
	{
		o << "Vector3(" << v.x << ", " << v.y << ", " << v.z << ")";
		return o;
	}

	//-----------------------------------//
};

//-----------------------------------//

typedef Vector3T<int> Vector3i;
typedef Vector3T<float> Vector3;
typedef Vector3T<double> Vector3d;

//-----------------------------------//

#endif // __Vector3_h__
