#include "scge\Math\Camera.h"

#include "scge\Math\Matrix3.h"

Camera::Camera()
	: mPosition(Vector3::Zero)
	, mUp(Vector3::UnitY)
	, mRight(Vector3::UnitX)
	, mLook(Vector3::UnitZ)
{
}

Matrix4 Camera::getViewMatrix()
{
	mLook.Normalise();

	mUp = mLook.crossProduct(mRight);
	mUp.Normalise();

	mRight = mUp.crossProduct(mLook);
	mRight.Normalise();

	float x = mRight.dotProduct(mPosition);
	float y = mUp.dotProduct(mPosition);
	float z = mLook.dotProduct(mPosition);

	Matrix4 ret;

	ret[0][0] = mRight.x;
	ret[0][1] = mUp.x;
	ret[0][2] = mLook.x;
	ret[0][3] = 0.0f;

	ret[1][0] = mRight.y;
	ret[1][1] = mUp.y;
	ret[1][2] = mLook.y;
	ret[1][3] = 0.0f;

	ret[2][0] = mRight.z;
	ret[2][1] = mUp.z;
	ret[2][2] = mLook.z;
	ret[2][3] = 0.0f;

	ret[3][0] = x;
	ret[3][1] = y;
	ret[3][2] = z;
	ret[3][3] = 1.0f;

	return ret;
}

void Camera::moveX(float dis)
{
	mPosition -= Vector3(mLook.x, 0.0f, mLook.z).CreateNormalised() * dis;
}

void Camera::moveY(float dis)
{
	mPosition.y -= dis;
}

void Camera::moveZ(float dis)
{
	mPosition -= Vector3(mRight.x, 0.0f, mRight.z).CreateNormalised() * dis;
}

void Camera::rotateX(float angle)
{
	Matrix3 mat(mRight, -angle);

	mUp = mUp * mat;
	mLook = mLook * mat;
}

void Camera::rotateY(float angle)
{
	Matrix3 mat(Vector3::UnitY, -angle);

	mRight = mRight * mat;
	mLook = mLook * mat;
}