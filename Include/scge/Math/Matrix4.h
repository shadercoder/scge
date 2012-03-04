#ifndef __Matrix4_h__
#define __Matrix4_h__

#include "scge\Math.h"

#include "scge\Math\Matrix3.h"
#include "scge\Math\Quaternion.h"
#include "scge\Math\Vector3.h"
#include "scge\Math\Vector4.h"
#include "scge\Math\Plane.h"

//-----------------------------------//

class Matrix4
{
public:

	Matrix4() { operator=(Identity); }

	inline Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33 )
	{
		m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
		m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
		m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
		m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
	}

	inline Matrix4(const Matrix3& m3x3)
	{
		operator=(Identity);
		operator=(m3x3);
	}

	inline Matrix4(const Quaternion& rot)
	{
		operator=(Identity);
		operator=(rot.ToRotationMatrix());
	}

	inline void swap(Matrix4& other)
	{
		std::swap(m[0][0], other.m[0][0]);
		std::swap(m[0][1], other.m[0][1]);
		std::swap(m[0][2], other.m[0][2]);
		std::swap(m[0][3], other.m[0][3]);

		std::swap(m[1][0], other.m[1][0]);
		std::swap(m[1][1], other.m[1][1]);
		std::swap(m[1][2], other.m[1][2]);
		std::swap(m[1][3], other.m[1][3]);

		std::swap(m[2][0], other.m[2][0]);
		std::swap(m[2][1], other.m[2][1]);
		std::swap(m[2][2], other.m[2][2]);
		std::swap(m[2][3], other.m[2][3]);

		std::swap(m[3][0], other.m[3][0]);
		std::swap(m[3][1], other.m[3][1]);
		std::swap(m[3][2], other.m[3][2]);
		std::swap(m[3][3], other.m[3][3]);
	}

	inline float* operator[](size_t iRow)
	{
		return m[iRow];
	}

	inline const float *operator[](size_t iRow) const
	{
		return m[iRow];
	}

	inline const Matrix4 &Concatenate(const Matrix4 &m2)
	{
		Matrix4 c(*this);

		m[0][0] = c.m[0][0] * m2.m[0][0] + c.m[0][1] * m2.m[1][0] + c.m[0][2] * m2.m[2][0] + c.m[0][3] * m2.m[3][0];
		m[0][1] = c.m[0][0] * m2.m[0][1] + c.m[0][1] * m2.m[1][1] + c.m[0][2] * m2.m[2][1] + c.m[0][3] * m2.m[3][1];
		m[0][2] = c.m[0][0] * m2.m[0][2] + c.m[0][1] * m2.m[1][2] + c.m[0][2] * m2.m[2][2] + c.m[0][3] * m2.m[3][2];
		m[0][3] = c.m[0][0] * m2.m[0][3] + c.m[0][1] * m2.m[1][3] + c.m[0][2] * m2.m[2][3] + c.m[0][3] * m2.m[3][3];

		m[1][0] = c.m[1][0] * m2.m[0][0] + c.m[1][1] * m2.m[1][0] + c.m[1][2] * m2.m[2][0] + c.m[1][3] * m2.m[3][0];
		m[1][1] = c.m[1][0] * m2.m[0][1] + c.m[1][1] * m2.m[1][1] + c.m[1][2] * m2.m[2][1] + c.m[1][3] * m2.m[3][1];
		m[1][2] = c.m[1][0] * m2.m[0][2] + c.m[1][1] * m2.m[1][2] + c.m[1][2] * m2.m[2][2] + c.m[1][3] * m2.m[3][2];
		m[1][3] = c.m[1][0] * m2.m[0][3] + c.m[1][1] * m2.m[1][3] + c.m[1][2] * m2.m[2][3] + c.m[1][3] * m2.m[3][3];

		m[2][0] = c.m[2][0] * m2.m[0][0] + c.m[2][1] * m2.m[1][0] + c.m[2][2] * m2.m[2][0] + c.m[2][3] * m2.m[3][0];
		m[2][1] = c.m[2][0] * m2.m[0][1] + c.m[2][1] * m2.m[1][1] + c.m[2][2] * m2.m[2][1] + c.m[2][3] * m2.m[3][1];
		m[2][2] = c.m[2][0] * m2.m[0][2] + c.m[2][1] * m2.m[1][2] + c.m[2][2] * m2.m[2][2] + c.m[2][3] * m2.m[3][2];
		m[2][3] = c.m[2][0] * m2.m[0][3] + c.m[2][1] * m2.m[1][3] + c.m[2][2] * m2.m[2][3] + c.m[2][3] * m2.m[3][3];
		
		m[3][0] = c.m[3][0] * m2.m[0][0] + c.m[3][1] * m2.m[1][0] + c.m[3][2] * m2.m[2][0] + c.m[3][3] * m2.m[3][0];
		m[3][1] = c.m[3][0] * m2.m[0][1] + c.m[3][1] * m2.m[1][1] + c.m[3][2] * m2.m[2][1] + c.m[3][3] * m2.m[3][1];
		m[3][2] = c.m[3][0] * m2.m[0][2] + c.m[3][1] * m2.m[1][2] + c.m[3][2] * m2.m[2][2] + c.m[3][3] * m2.m[3][2];
		m[3][3] = c.m[3][0] * m2.m[0][3] + c.m[3][1] * m2.m[1][3] + c.m[3][2] * m2.m[2][3] + c.m[3][3] * m2.m[3][3];

		return *this;
	}

	inline Matrix4 ConcatenatedCopy(const Matrix4 &m2) const
	{
		return Matrix4(*this).Concatenate(m2);
	}

	inline Matrix4 operator*(const Matrix4 &m2) const
	{
		return ConcatenatedCopy(m2);
	}

	inline Vector3 operator*(const Vector3 &v) const
	{
		Vector3 r;

		float fInvW = 1.0f / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

		r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
		r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
		r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

		return r;
	}

	inline Vector4 operator*(const Vector4& v) const
	{
		return Vector4(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
			m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
			);
	}

	inline Plane operator*(const Plane& p) const
	{
		Plane ret;
		Matrix4 invTrans = InversedCopy().TransposedCopy();
		Vector4 v4( p.normal.x, p.normal.y, p.normal.z, p.offset );
		v4 = invTrans * v4;
		ret.normal.x = v4.x; 
		ret.normal.y = v4.y; 
		ret.normal.z = v4.z;
		ret.offset = v4.w / ret.normal.Normalise();

		return ret;
	}

	inline Matrix4 operator+(const Matrix4 &m2) const
	{
		Matrix4 r;

		r.m[0][0] = m[0][0] + m2.m[0][0];
		r.m[0][1] = m[0][1] + m2.m[0][1];
		r.m[0][2] = m[0][2] + m2.m[0][2];
		r.m[0][3] = m[0][3] + m2.m[0][3];

		r.m[1][0] = m[1][0] + m2.m[1][0];
		r.m[1][1] = m[1][1] + m2.m[1][1];
		r.m[1][2] = m[1][2] + m2.m[1][2];
		r.m[1][3] = m[1][3] + m2.m[1][3];

		r.m[2][0] = m[2][0] + m2.m[2][0];
		r.m[2][1] = m[2][1] + m2.m[2][1];
		r.m[2][2] = m[2][2] + m2.m[2][2];
		r.m[2][3] = m[2][3] + m2.m[2][3];

		r.m[3][0] = m[3][0] + m2.m[3][0];
		r.m[3][1] = m[3][1] + m2.m[3][1];
		r.m[3][2] = m[3][2] + m2.m[3][2];
		r.m[3][3] = m[3][3] + m2.m[3][3];

		return r;
	}

	inline Matrix4 operator-(const Matrix4 &m2) const
	{
		Matrix4 r;

		r.m[0][0] = m[0][0] - m2.m[0][0];
		r.m[0][1] = m[0][1] - m2.m[0][1];
		r.m[0][2] = m[0][2] - m2.m[0][2];
		r.m[0][3] = m[0][3] - m2.m[0][3];

		r.m[1][0] = m[1][0] - m2.m[1][0];
		r.m[1][1] = m[1][1] - m2.m[1][1];
		r.m[1][2] = m[1][2] - m2.m[1][2];
		r.m[1][3] = m[1][3] - m2.m[1][3];

		r.m[2][0] = m[2][0] - m2.m[2][0];
		r.m[2][1] = m[2][1] - m2.m[2][1];
		r.m[2][2] = m[2][2] - m2.m[2][2];
		r.m[2][3] = m[2][3] - m2.m[2][3];

		r.m[3][0] = m[3][0] - m2.m[3][0];
		r.m[3][1] = m[3][1] - m2.m[3][1];
		r.m[3][2] = m[3][2] - m2.m[3][2];
		r.m[3][3] = m[3][3] - m2.m[3][3];

		return r;
	}

	inline bool operator==(const Matrix4& m2) const
	{
		if( 
			m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
			m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
			m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
			m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
			return false;

		return true;
	}

	inline bool operator!=(const Matrix4& m2) const
	{
		if( 
			m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
			m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
			m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
			m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
			return true;

		return false;
	}

	inline void operator=(const Matrix3& mat3)
	{
		m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
		m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
		m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
	}

	inline Matrix4 &Transpose()
	{
		std::swap(m[1][0], m[0][1]);
		std::swap(m[2][0], m[0][2]);
		std::swap(m[3][0], m[0][3]);
		std::swap(m[2][1], m[1][2]);
		std::swap(m[3][1], m[1][3]);
		std::swap(m[3][2], m[2][3]);

		return *this;
	}

	inline Matrix4 TransposedCopy() const
	{
		return Matrix4(*this).Transpose();
	}

	Matrix4 &Inverse();
	inline Matrix4 InversedCopy() const
	{
		return Matrix4(*this).Inverse();
	}

	inline void setTrans(const Vector3& v)
	{
		m[3][0] = v.x;
		m[3][1] = v.y;
		m[3][2] = v.z;
	}

	inline Vector3 getTrans() const
	{
		return Vector3(m[3][0], m[3][1], m[3][2]);
	}

	inline void setScale(const Vector3& v)
	{
		m[0][0] = v.x;
		m[1][1] = v.y;
		m[2][2] = v.z;
	}

	inline Vector3 getScale() const
	{
		return Vector3(m[0][0], m[1][1], m[2][2]);
	}

	inline static Matrix4 MakeTranslate(const Vector3& v)
	{
		Matrix4 ret;

		ret.m[0][0] = 1.0; ret.m[0][1] = 0.0; ret.m[0][2] = 0.0; ret.m[0][3] = 0.0;
		ret.m[1][0] = 0.0; ret.m[1][1] = 1.0; ret.m[1][2] = 0.0; ret.m[1][3] = 0.0;
		ret.m[2][0] = 0.0; ret.m[2][1] = 0.0; ret.m[2][2] = 1.0; ret.m[2][3] = 0.0;
		ret.m[3][0] = v.x; ret.m[3][1] = v.y; ret.m[3][2] = v.z; ret.m[3][3] = 1.0;

		return ret;
	}

	inline static Matrix4 MakeScale(const Vector3& v)
	{
		Matrix4 ret;

		ret.m[0][0] = v.x; ret.m[0][1] = 0.0; ret.m[0][2] = 0.0; ret.m[0][3] = 0.0;
		ret.m[1][0] = 0.0; ret.m[1][1] = v.y; ret.m[1][2] = 0.0; ret.m[1][3] = 0.0;
		ret.m[2][0] = 0.0; ret.m[2][1] = 0.0; ret.m[2][2] = v.z; ret.m[2][3] = 0.0;
		ret.m[3][0] = 0.0; ret.m[3][1] = 0.0; ret.m[3][2] = 0.0; ret.m[3][3] = 1.0;

		return ret;
	}

	inline Matrix3 extract3x3Matrix() const
	{
		Matrix3 m3x3;
		m3x3.m[0][0] = m[0][0];
		m3x3.m[0][1] = m[0][1];
		m3x3.m[0][2] = m[0][2];
		m3x3.m[1][0] = m[1][0];
		m3x3.m[1][1] = m[1][1];
		m3x3.m[1][2] = m[1][2];
		m3x3.m[2][0] = m[2][0];
		m3x3.m[2][1] = m[2][1];
		m3x3.m[2][2] = m[2][2];

		return m3x3;
	}

	inline bool hasScale() const
	{
		float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
		if(!Math::Equal(t, 1.0))
			return true;

		t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
		if(!Math::Equal(t, 1.0))
			return true;

		t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
		if(!Math::Equal(t, 1.0))
			return true;

		return false;
	}

	inline bool hasNegativeScale() const
	{
		return determinant() < 0;
	}

	inline Quaternion extractQuaternion() const
	{
		return Quaternion(extract3x3Matrix());
	}

	inline Matrix4 operator*(float scalar) const
	{
		return Matrix4(
			scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
			scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
			scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
			scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
	}

	inline friend std::ostream& operator<<(std::ostream& o, const Matrix4& m)
	{
		o << "Matrix4(";
		for (size_t i = 0; i < 4; ++i)
		{
			o << " row" << (unsigned)i << "{";
			for(size_t j = 0; j < 4; ++j)
			{
				o << m[i][j] << " ";
			}
			o << "}";
		}
		o << ")";
		return o;
	}

	Matrix4 adjoint() const;
	float determinant() const;

	void makeTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);
	void makeInverseTransform(const Vector3& position, const Vector3& scale, const Quaternion& orientation);
	void decomposition(Vector3& position, Vector3& scale, Quaternion& orientation) const;

	inline bool isAffine() const
	{
		return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
	}

	Matrix4 inverseAffine(void) const;

	inline Matrix4 concatenateAffine(const Matrix4 &m2) const
	{
		return Matrix4(
			m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0],
			m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1],
			m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2],
			m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3],

			m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0],
			m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1],
			m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2],
			m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3],

			m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0],
			m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1],
			m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2],
			m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3],

			0, 0, 0, 1);
	}

	inline Vector3 transformAffine(const Vector3& v) const
	{
		return Vector3(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3], 
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
	}

	inline Vector4 transformAffine(const Vector4& v) const
	{
		return Vector4(
			m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
			m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
			m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
			v.w);
	}

	static const Matrix4 Zero;
	static const Matrix4 Identity;
	static const Matrix4 ClipSpace2DToImageSpace;

protected:
	union {
		float m[4][4];
		float _m[16];
	};
};

inline Vector3 operator*(const Vector3 &v, const Matrix4& mat)
{
	Vector3 r;

	float fInvW = 1.0f / (v.x*mat[0][3] + v.y*mat[1][3] + v.z*mat[2][3] + mat[3][3]);

	r.x = (v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0] + mat[3][0]) * fInvW;
	r.y = (v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1] + mat[3][1]) * fInvW;
	r.z = (v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2] + mat[3][2]) * fInvW;

	return r;
}

inline Vector4 operator*(const Vector4& v, const Matrix4& mat)
{
	return Vector4(
		v.x*mat[0][0] + v.y*mat[1][0] + v.z*mat[2][0] + v.w*mat[3][0],
		v.x*mat[0][1] + v.y*mat[1][1] + v.z*mat[2][1] + v.w*mat[3][1],
		v.x*mat[0][2] + v.y*mat[1][2] + v.z*mat[2][2] + v.w*mat[3][2],
		v.x*mat[0][3] + v.y*mat[1][3] + v.z*mat[2][3] + v.w*mat[3][3]
	);
}

#endif // __Matrix4_h__