#ifndef Camera_h__
#define Camera_h__

#include "scge\Math.h"

#include "scge\Math\Vector3.h"

//-----------------------------------//

class Camera
{
public:
	Camera();

	const Vector3 &getPosition() const { return mPosition; }
	void setPosition(Vector3 position) { mPosition = position; }
	void setPosition(float x, float y, float z) { mPosition = Vector3(x, y, z); }

	Matrix4 getViewMatrix();

	void moveX(float dis);
	void moveY(float dis);
	void moveZ(float dis);

	void rotateX(float angle);
	void rotateY(float angle);

	const Vector3 &getRight() const { return mRight; }
	const Vector3 &getUp() const { return mUp; }
	const Vector3 &getLook() const { return mLook; }

private:
	Vector3 mPosition;

	Vector3 mRight;
	Vector3 mUp;
	Vector3 mLook;
};

//-----------------------------------//

#endif // Camera_h__