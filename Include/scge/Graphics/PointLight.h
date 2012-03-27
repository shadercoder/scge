#ifndef __PointLight_h__
#define __PointLight_h__

#include "scge\Math.h"
#include "scge\Math\Vector3.h"

class Renderer3D;
class PointLight
{
public:
	PointLight(Renderer3D &renderer3D, const Vector3 &position, const Vector3 &colour, float attenuationBegin, float attenuationEnd);
	~PointLight();

	PointLight(const PointLight &pointLight);
	PointLight &operator=(const PointLight &pointLight);

	const Vector3 &getPosition() const { return mPosition; }
	void setPosition(const Vector3 &position) { mPosition = position; }

	const Vector3 &getColour() const { return mColour; }
	void setColour(const Vector3 &colour) { mColour = colour; }

	float getAttenuationBegin() const { return mAttenuationBegin; }
	void setAttenuationBegin(float attenuationBegin) { mAttenuationBegin = attenuationBegin; }

	float getAttenuationEnd() const { return mAttenuationEnd; }
	void setAttenuationEnd(float attenuationEnd) { mAttenuationEnd = attenuationEnd; }

	void Release();

private:
	Renderer3D *mRenderer3D;

	Vector3 mPosition;
	Vector3 mColour;
	float mAttenuationBegin;
	float mAttenuationEnd;
};

#endif // __PointLight_h__