#include "scge\Math\Quaternion.h"

#include "scge\Math\Matrix3.h"
#include "scge\Math\Vector3.h"

//-----------------------------------//

const Quaternion Quaternion::Identity(1, 0, 0, 0);
const Quaternion Quaternion::Zero(0, 0, 0, 0);

//-----------------------------------//

void Quaternion::FromRotationMatrix(const Matrix3& kRot)
{
	float fTrace = kRot[0][0] + kRot[1][1] + kRot[2][2];

	if(fTrace > 0.0)
	{
		float root = Math::Sqrt(fTrace + 1.0f);
		w = 0.5f * root;
		root = 0.5f / root;

		x = (kRot[2][1] - kRot[1][2]) * root;
		y = (kRot[0][2] - kRot[2][0]) * root;
		z = (kRot[1][0] - kRot[0][1]) * root;
	}
	else
	{
		static size_t s_iNext[3] = { 1, 2, 0 };
		size_t i = 0;

		if(kRot[1][1] > kRot[0][0])
			i = 1;

		if(kRot[2][2] > kRot[i][i])
			i = 2;

		size_t j = s_iNext[i];
		size_t k = s_iNext[j];

		float root = Math::Sqrt(kRot[i][i] - kRot[j][j] - kRot[k][k] + 1.0f);
		float* apkQuat[3] = { &x, &y, &z };

		*apkQuat[i] = 0.5f * root;
		root = 0.5f / root;
		w = (kRot[k][j] - kRot[j][k]) * root;
		*apkQuat[j] = (kRot[j][i] + kRot[i][j]) * root;
		*apkQuat[k] = (kRot[k][i] + kRot[i][k]) * root;
	}
}

//-----------------------------------//

Matrix3 Quaternion::ToRotationMatrix() const
{
	Matrix3 ret;

	float fTx  = x+x;
	float fTy  = y+y;
	float fTz  = z+z;
	float fTwx = fTx*w;
	float fTwy = fTy*w;
	float fTwz = fTz*w;
	float fTxx = fTx*x;
	float fTxy = fTy*x;
	float fTxz = fTz*x;
	float fTyy = fTy*y;
	float fTyz = fTz*y;
	float fTzz = fTz*z;

	ret[0][0] = 1.0f-(fTyy+fTzz);
	ret[0][1] = fTxy-fTwz;
	ret[0][2] = fTxz+fTwy;
	ret[1][0] = fTxy+fTwz;
	ret[1][1] = 1.0f-(fTxx+fTzz);
	ret[1][2] = fTyz-fTwx;
	ret[2][0] = fTxz-fTwy;
	ret[2][1] = fTyz+fTwx;
	ret[2][2] = 1.0f-(fTxx+fTyy);

	return ret;
}

//-----------------------------------//

void Quaternion::FromAngleAxis(const float& rfAngle, const Vector3& rkAxis)
{
	float halfAngle = 0.5f * rfAngle;
	float fSin = Math::Sin(halfAngle);

	w = Math::Cos(halfAngle);
	x = fSin * rkAxis.x;
	y = fSin * rkAxis.y;
	z = fSin * rkAxis.z;
}

//-----------------------------------//

void Quaternion::ToAngleAxis(float& rfAngle, Vector3& rkAxis) const
{
	float sqrLength = x*x + y*y + z*z;
	if(sqrLength > 0.0f)
	{
		rfAngle = 2.0f * Math::ACos(w);
		float fInvLength = Math::InvSqrt(sqrLength);
		rkAxis.x = x*fInvLength;
		rkAxis.y = y*fInvLength;
		rkAxis.z = z*fInvLength;
	}
	else
	{
		rfAngle = 0.0f;
		rkAxis.x = 1.0f;
		rkAxis.y = 0.0f;
		rkAxis.z = 0.0f;
	}
}

//-----------------------------------//

void Quaternion::FromAxes(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
{
	Matrix3 kRot;

	kRot[0][0] = xaxis.x;
	kRot[1][0] = xaxis.y;
	kRot[2][0] = xaxis.z;

	kRot[0][1] = yaxis.x;
	kRot[1][1] = yaxis.y;
	kRot[2][1] = yaxis.z;

	kRot[0][2] = zaxis.x;
	kRot[1][2] = zaxis.y;
	kRot[2][2] = zaxis.z;

	FromRotationMatrix(kRot);

}

//-----------------------------------//

Vector3 Quaternion::getXAxis() const
{
	float fTy  = 2.0f*y;
	float fTz  = 2.0f*z;
	float fTwy = fTy*w;
	float fTwz = fTz*w;
	float fTxy = fTy*x;
	float fTxz = fTz*x;
	float fTyy = fTy*y;
	float fTzz = fTz*z;

	return Vector3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
}

//-----------------------------------//

Vector3 Quaternion::getYAxis() const
{
	float fTx  = 2.0f*x;
	float fTy  = 2.0f*y;
	float fTz  = 2.0f*z;
	float fTwx = fTx*w;
	float fTwz = fTz*w;
	float fTxx = fTx*x;
	float fTxy = fTy*x;
	float fTyz = fTz*y;
	float fTzz = fTz*z;

	return Vector3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
}

//-----------------------------------//

Vector3 Quaternion::getZAxis() const
{
	float fTx  = 2.0f*x;
	float fTy  = 2.0f*y;
	float fTz  = 2.0f*z;
	float fTwx = fTx*w;
	float fTwy = fTy*w;
	float fTxx = fTx*x;
	float fTxz = fTz*x;
	float fTyy = fTy*y;
	float fTyz = fTz*y;

	return Vector3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
}

//-----------------------------------//

void Quaternion::ToAxes(Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const
{
	Matrix3 kRot = ToRotationMatrix();

	xaxis.x = kRot[0][0];
	xaxis.y = kRot[1][0];
	xaxis.z = kRot[2][0];

	yaxis.x = kRot[0][1];
	yaxis.y = kRot[1][1];
	yaxis.z = kRot[2][1];

	zaxis.x = kRot[0][2];
	zaxis.y = kRot[1][2];
	zaxis.z = kRot[2][2];
}

//-----------------------------------//

Quaternion Quaternion::operator+(const Quaternion& other) const
{
	return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
}

//-----------------------------------//

Quaternion Quaternion::operator-(const Quaternion& other) const
{
	return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
}

//-----------------------------------//

Quaternion Quaternion::operator*(const Quaternion& other) const
{
	return Quaternion(
		w * other.w - x * other.x - y * other.y - z * other.z,
		w * other.x + x * other.w + y * other.z - z * other.y,
		w * other.y + y * other.w + z * other.x - x * other.z,
		w * other.z + z * other.w + x * other.y - y * other.x);
}

//-----------------------------------//

Quaternion Quaternion::operator*(float fScalar) const
{
	return Quaternion(fScalar * w, fScalar * x, fScalar * y, fScalar * z);
}

//-----------------------------------//

Quaternion operator*(float fScalar, const Quaternion& rkQ)
{
	return Quaternion(fScalar * rkQ.w, fScalar * rkQ.x, fScalar * rkQ.y, fScalar * rkQ.z);
}

//-----------------------------------//

Quaternion Quaternion::operator-() const
{
	return Quaternion(-w, -x, -y, -z);
}

//-----------------------------------//

float Quaternion::Dot(const Quaternion& rkQ) const
{
	return w * rkQ.w + x * rkQ.x + y * rkQ.y+z * rkQ.z;
}

//-----------------------------------//

Quaternion Quaternion::getInverse() const
{
	float fNorm = LengthSquared();
	if(fNorm > 0.0)
	{
		float fInvNorm = 1.0f / fNorm;
		return Quaternion(w*fInvNorm, -x*fInvNorm, -y*fInvNorm, -z*fInvNorm);
	}
	else
		return Zero;
}

//-----------------------------------//

Quaternion Quaternion::getConjugate() const
{
	return Quaternion(w, -x, -y, -z);
}

//-----------------------------------//

Quaternion Quaternion::Exp()const
{
	float fAngle = Math::Sqrt(x*x + y*y + z*z);
	float fSin = Math::Sin(fAngle);

	Quaternion kResult;
	kResult.w = Math::Cos(fAngle);

	if(Math::Abs(fSin) >= Math::Limits::FloatEpsilon)
	{
		float fCoeff = fSin/(fAngle);
		kResult.x = fCoeff*x;
		kResult.y = fCoeff*y;
		kResult.z = fCoeff*z;
	}
	else
	{
		kResult.x = x;
		kResult.y = y;
		kResult.z = z;
	}

	return kResult;
}

//-----------------------------------//

Quaternion Quaternion::Log() const
{
	Quaternion kResult;
	kResult.w = 0.0;

	if(Math::Abs(w) < 1.0)
	{
		float fAngle = Math::ACos(w);
		float fSin = Math::Sin(fAngle);
		if(Math::Abs(fSin) >= Math::Limits::FloatEpsilon)
		{
			float fCoeff = fAngle/fSin;
			kResult.x = fCoeff*x;
			kResult.y = fCoeff*y;
			kResult.z = fCoeff*z;
			return kResult;
		}
	}

	kResult.x = x;
	kResult.y = y;
	kResult.z = z;

	return kResult;
}

//-----------------------------------//

Vector3 Quaternion::operator*(const Vector3& v) const
{
	Vector3 uv, uuv;
	Vector3 qvec(x, y, z);
	uv = qvec.crossProduct(v);
	uuv = qvec.crossProduct(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;
}

Vector3 operator*(const Vector3& rkVector, const Quaternion& rkQ)
{
	return rkQ * rkVector;
}

//-----------------------------------//

bool Quaternion::equals(const Quaternion& rhs, const float& tolerance) const
{
	float fCos = Dot(rhs);
	float angle = Math::ACos(fCos);

	return (Math::Abs(angle) <= tolerance) || Math::Equal(angle, Math::Constants::Pi, tolerance);
}

//-----------------------------------//

Quaternion Quaternion::Slerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath)
{
	float fCos = rkP.Dot(rkQ);
	Quaternion rkT;

	if (fCos < 0.0f && shortestPath)
	{
		fCos = -fCos;
		rkT = -rkQ;
	}
	else
		rkT = rkQ;

	if (Math::Abs(fCos) < 1 - Math::Limits::FloatEpsilon)
	{
		float fSin = Math::Sqrt(1 - Math::Sqr(fCos));
		float fAngle = Math::ATan2(fSin, fCos);
		float fInvSin = 1.0f / fSin;
		float fCoeff0 = Math::Sin((1.0f - fT) * fAngle) * fInvSin;
		float fCoeff1 = Math::Sin(fT * fAngle) * fInvSin;

		return fCoeff0 * rkP + fCoeff1 * rkT;
	}
	else
	{
		Quaternion t = (1.0f - fT) * rkP + fT * rkT;
		t.Normalise();

		return t;
	}
}

//-----------------------------------//

Quaternion Quaternion::SlerpExtraSpins(float fT, const Quaternion& rkP, const Quaternion& rkQ, int iExtraSpins)
{
	float fCos = rkP.Dot(rkQ);
	float fAngle = Math::ACos(fCos);

	if(Math::Abs(fAngle) < Math::Limits::FloatEpsilon)
		return rkP;

	float fSin = Math::Sin(fAngle);
	float fPhase = Math::Constants::Pi*iExtraSpins*fT;
	float fInvSin = 1.0f/fSin;
	float fCoeff0 = Math::Sin((1.0f-fT)*fAngle - fPhase)*fInvSin;
	float fCoeff1 = Math::Sin(fT*fAngle + fPhase)*fInvSin;
	return fCoeff0*rkP + fCoeff1*rkQ;
}

//-----------------------------------//

void Quaternion::Intermediate(const Quaternion& rkQ0, const Quaternion& rkQ1, const Quaternion& rkQ2, Quaternion& rkA, Quaternion& rkB)
{
	Quaternion kQ0inv = rkQ0.getConjugate();
	Quaternion kQ1inv = rkQ1.getConjugate();
	Quaternion rkP0 = kQ0inv*rkQ1;
	Quaternion rkP1 = kQ1inv*rkQ2;
	Quaternion kArg = 0.25*(rkP0.Log()-rkP1.Log());
	Quaternion kMinusArg = -kArg;

	rkA = rkQ1*kArg.Exp();
	rkB = rkQ1*kMinusArg.Exp();
}

//-----------------------------------//

Quaternion Quaternion::Squad(float fT, const Quaternion& rkP, const Quaternion& rkA, const Quaternion& rkB, const Quaternion& rkQ, bool shortestPath)
{
	float fSlerpT = 2.0f*fT*(1.0f-fT);
	Quaternion kSlerpP = Slerp(fT, rkP, rkQ, shortestPath);
	Quaternion kSlerpQ = Slerp(fT, rkA, rkB);
	return Slerp(fSlerpT, kSlerpP ,kSlerpQ);
}

//-----------------------------------//

float Quaternion::Normalise()
{
	float len = LengthSquared();
	if(fabs(len) > Math::Limits::FloatEpsilon && fabs(len - 1.0f) > Math::Limits::FloatEpsilon)
	{
		float mag = sqrt(len);
		w /= mag;
		x /= mag;
		y /= mag;
		z /= mag;
	}

	return len;
}

//-----------------------------------//

float Quaternion::getRoll(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwz = fTz*w;
		float fTxy = fTy*x;
		float fTyy = fTy*y;
		float fTzz = fTz*z;

		return Math::ATan2(fTxy+fTwz, 1.0f-(fTyy+fTzz));
	}
	else
		return Math::ATan2(2*(x*y + w*z), w*w + x*x - y*y - z*z);
}

//-----------------------------------//

float Quaternion::getPitch(bool reprojectAxis) const
{
	if(reprojectAxis)
	{
		float fTx  = 2.0f*x;
		float fTz  = 2.0f*z;
		float fTwx = fTx*w;
		float fTxx = fTx*x;
		float fTyz = fTz*y;
		float fTzz = fTz*z;

		return Math::ATan2(fTyz+fTwx, 1.0f-(fTxx+fTzz));
	}
	else
		return Math::ATan2(2*(y*z + w*x), w*w - x*x - y*y + z*z);
}

//-----------------------------------//

float Quaternion::getYaw(bool reprojectAxis) const
{
	if (reprojectAxis)
	{
		float fTx  = 2.0f*x;
		float fTy  = 2.0f*y;
		float fTz  = 2.0f*z;
		float fTwy = fTy*w;
		float fTxx = fTx*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;

		return Math::ATan2(fTxz+fTwy, 1.0f-(fTxx+fTyy));
	}
	else
		return Math::ASin(-2*(x*z - w*y));
}

//-----------------------------------//

Quaternion Quaternion::nlerp(float fT, const Quaternion& rkP, const Quaternion& rkQ, bool shortestPath)
{
	Quaternion result;
	float fCos = rkP.Dot(rkQ);
	if (fCos < 0.0f && shortestPath)
	{
		result = rkP + fT * ((-rkQ) - rkP);
	}
	else
	{
		result = rkP + fT * (rkQ - rkP);
	}
	result.Normalise();
	return result;
}

//-----------------------------------//