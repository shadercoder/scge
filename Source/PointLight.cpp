#include "scge\Graphics\PointLight.h"

#include "scge\Graphics\Renderer3D.h"

PointLight::PointLight(Renderer3D &renderer3D, const Vector3 &position, const Vector3 &colour, float attenuationBegin, float attenuationEnd)
	: mRenderer3D(&renderer3D)
	, mPosition(position)
	, mColour(colour)
	, mAttenuationBegin(attenuationBegin)
	, mAttenuationEnd(attenuationEnd)
{
	mRenderer3D->registerPointLight(*this);
}

PointLight::~PointLight()
{
	mRenderer3D->unRegisterPointLight(*this);
}

PointLight::PointLight(const PointLight &pointLight)
	: mRenderer3D(pointLight.mRenderer3D)
	, mPosition(pointLight.mPosition)
	, mColour(pointLight.mColour)
	, mAttenuationBegin(pointLight.mAttenuationBegin)
	, mAttenuationEnd(pointLight.mAttenuationEnd)
{
	mRenderer3D->registerPointLight(*this);
}

PointLight &PointLight::operator=(const PointLight &pointLight)
{
	mRenderer3D->unRegisterPointLight(*this);

	mRenderer3D = pointLight.mRenderer3D;
	mPosition = pointLight.mPosition;
	mColour = pointLight.mColour;
	mAttenuationBegin = pointLight.mAttenuationBegin;
	mAttenuationEnd = pointLight.mAttenuationEnd;

	mRenderer3D->registerPointLight(*this);

	return *this;
}

void PointLight::Release()
{
	if(mRenderer3D)
		mRenderer3D->registerPointLight(*this);
}