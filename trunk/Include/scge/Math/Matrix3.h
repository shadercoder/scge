#ifndef __Matrix3_h__
#define __Matrix3_h__

#include "scge\Math.h"

//-----------------------------------//

class Matrix3
{
public:

	Matrix3() { operator=(Identity); }

	Matrix3(const Vector3& rkAxis, const float& fRadians)
	{
		FromAxisAngle(rkAxis, fRadians);
	}

	inline explicit Matrix3(const float arr[3][3])
	{
		memcpy(m, arr, 9 * sizeof(float));
	}

	inline Matrix3(const Matrix3& rkMatrix)
	{
		memcpy(m, rkMatrix.m, 9 * sizeof(float));
	}

	Matrix3 (float fEntry00, float fEntry01, float fEntry02,
		float fEntry10, float fEntry11, float fEntry12,
		float fEntry20, float fEntry21, float fEntry22)
	{
		m[0][0] = fEntry00;
		m[0][1] = fEntry01;
		m[0][2] = fEntry02;
		m[1][0] = fEntry10;
		m[1][1] = fEntry11;
		m[1][2] = fEntry12;
		m[2][0] = fEntry20;
		m[2][1] = fEntry21;
		m[2][2] = fEntry22;
	}

	inline void swap(Matrix3& other)
	{
		std::swap(m[0][0], other.m[0][0]);
		std::swap(m[0][1], other.m[0][1]);
		std::swap(m[0][2], other.m[0][2]);
		std::swap(m[1][0], other.m[1][0]);
		std::swap(m[1][1], other.m[1][1]);
		std::swap(m[1][2], other.m[1][2]);
		std::swap(m[2][0], other.m[2][0]);
		std::swap(m[2][1], other.m[2][1]);
		std::swap(m[2][2], other.m[2][2]);
	}

	inline float* operator[](size_t iRow) const
	{
		return (float*)m[iRow];
	}

	Vector3 GetColumn (size_t iCol) const;
	void SetColumn(size_t iCol, const Vector3& vec);
	void FromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);

	inline Matrix3& operator=(const Matrix3& rkMatrix)
	{
		memcpy(m, rkMatrix.m, 9 * sizeof(float));
		return *this;
	}
	bool operator==(const Matrix3& rkMatrix) const;
	inline bool operator!=(const Matrix3& rkMatrix) const
	{
		return !operator==(rkMatrix);
	}

	Matrix3 operator+(const Matrix3& rkMatrix) const;
	Matrix3 operator-(const Matrix3& rkMatrix) const;
	Matrix3 operator*(const Matrix3& rkMatrix) const;
	Matrix3 operator-() const;

	Vector3 operator*(const Vector3& rkVector) const;

	friend Vector3 operator*(const Vector3& rkVector, const Matrix3& rkMatrix);

	Matrix3 operator*(float fScalar) const;

	friend Matrix3 operator*(float fScalar, const Matrix3& rkMatrix);

	Matrix3 Transpose() const;
	bool Inverse(Matrix3& rkInverse, float fTolerance = Math::Limits::FloatEpsilon) const;
	Matrix3 Inverse(float fTolerance = Math::Limits::FloatEpsilon) const;
	float Determinant() const;

	void SingularValueDecomposition(Matrix3& rkL, Vector3& rkS, Matrix3& rkR) const;
	void SingularValueComposition(const Matrix3& rkL, const Vector3& rkS, const Matrix3& rkR);

	void Orthonormalize();

	void QDUDecomposition (Matrix3& rkQ, Vector3& rkD, Vector3& rkU) const;

	float SpectralNorm () const;

	void ToAxisAngle(Vector3& rkAxis, float& rfAngle) const;
	void FromAxisAngle(const Vector3& rkAxis, const float& fRadians);

	EulerAngles ToEulerAnglesXYZ() const;
	void FromEulerAnglesXYZ(const EulerAngles& angles);


	void EigenSolveSymmetric(float afEigenvalue[3], Vector3 akEigenvector[3]) const;

	static void TensorProduct(const Vector3& rkU, const Vector3& rkV, Matrix3& rkProduct);

	inline bool hasScale() const
	{
		float t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
		if (!Math::Equal(t, 1.0))
			return true;
		t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
		if (!Math::Equal(t, 1.0))
			return true;
		t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
		if (!Math::Equal(t, 1.0))
			return true;

		return false;
	}

	/** Function for writing to a stream.
	*/
	inline friend std::ostream& operator<<(std::ostream& o, const Matrix3& m)
	{
		o << "Matrix3(" << m[0][0] << ", " << m[0][1] << ", " << m[0][2] << ", " 
						<< m[1][0] << ", " << m[1][1] << ", " << m[1][2] << ", " 
						<< m[2][0] << ", " << m[2][1] << ", " << m[2][2] << ")";
		return o;
	}

	static const Matrix3 Zero;
	static const Matrix3 Identity;

protected:
	void Tridiagonal (float afDiag[3], float afSubDiag[3]);
	bool QLAlgorithm (float afDiag[3], float afSubDiag[3]);

	static void Bidiagonalize (Matrix3& kA, Matrix3& kL, Matrix3& kR);
	static void GolubKahanStep (Matrix3& kA, Matrix3& kL, Matrix3& kR);

	static float MaxCubicRoot (float afCoeff[3]);

	float m[3][3];

	friend class Matrix4;
};

//-----------------------------------//

#endif // __Matrix3_h__