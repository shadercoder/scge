#include "scge\Math\Frustum.h"

#include "scge\Math\Vector2.h"
#include "scge\Math\BoundingSphere.h"

//-----------------------------------//

Frustum::Frustum()
	: projection(ProjectionType::Perspective)
	, fieldOfView(60)
	, nearPlane(1)
	, farPlane(1000)
	, aspectRatio(1)
{ }

//-----------------------------------//

Frustum::Frustum(const Frustum& other)
	: projection(other.projection)
	, matProjection(other.matProjection)
	, fieldOfView(other.fieldOfView)
	, nearPlane(other.nearPlane)
	, farPlane(other.farPlane)
	, aspectRatio(other.aspectRatio)
{
	for(unsigned int i = 0; i < 6; i++ )
		planes[i] = other.planes[i];
}

//-----------------------------------//

void Frustum::updateProjection(const Vector2& size)
{
	if(projection == ProjectionType::Perspective)
		matProjection = Math::CreatePerspectiveProjection(fieldOfView, aspectRatio, nearPlane, farPlane);
	else
		matProjection = Math::CreateOrthographicProjection(size.x, size.y, nearPlane, farPlane);
}

//-----------------------------------//

void Frustum::updatePlanes(const Matrix4& matView)
{
	Matrix4 matClip = matView * matProjection;

	// Left clipping plane
	planes[0].normal.x = matClip[0][3] + matClip[0][0];
	planes[0].normal.y = matClip[1][3] + matClip[1][0];
	planes[0].normal.z = matClip[2][3] + matClip[2][0];
	planes[0].offset   = matClip[3][3] + matClip[3][0];
	planes[0].normalise();

	// Right clipping plane
	planes[1].normal.x = matClip[0][3] - matClip[0][1];
	planes[1].normal.y = matClip[1][3] - matClip[1][1];
	planes[1].normal.z = matClip[2][3] - matClip[2][1];
	planes[1].offset   = matClip[3][3] - matClip[3][1];
	planes[1].normalise();

	// Top clipping plane
	planes[2].normal.x = matClip[0][3] - matClip[0][1];
	planes[2].normal.y = matClip[1][3] - matClip[1][1];
	planes[2].normal.z = matClip[2][3] - matClip[2][1];
	planes[2].offset   = matClip[3][3] - matClip[3][1];
	planes[2].normalise();

	// Bottom clipping plane
	planes[3].normal.x = matClip[0][3] + matClip[0][1];
	planes[3].normal.y = matClip[1][3] + matClip[1][1];
	planes[3].normal.z = matClip[2][3] + matClip[2][1];
	planes[3].offset   = matClip[3][3] + matClip[3][1];
	planes[3].normalise();

	// Near clipping plane
	planes[4].normal.x = matClip[0][3] + matClip[0][2];
	planes[4].normal.y = matClip[0][3] + matClip[1][2];
	planes[4].normal.z = matClip[0][3] + matClip[2][2];
	planes[4].offset   = matClip[0][3] + matClip[3][2];
	planes[4].normalise();

	// Far clipping plane
	planes[5].normal.x = matClip[0][3] - matClip[0][2];
	planes[5].normal.y = matClip[1][3] - matClip[1][2];
	planes[5].normal.z = matClip[2][3] - matClip[2][2];
	planes[5].offset   = matClip[3][3] - matClip[3][2];
	planes[5].normalise();

	updateCorners(matView);
}

//-----------------------------------//

void Frustum::updateCorners(const Matrix4& matView)
{
	Matrix4 matClip = matView * matProjection;
	Matrix4 matInvClip = matClip.InversedCopy();

	Vector3 cornerPoints[] =
	{
		Vector3(-1,  1,  1), Vector3(1,  1,  1),
		Vector3(-1, -1,  1), Vector3(1, -1,  1),
		Vector3(-1,  1, -1), Vector3(1,  1, -1),
		Vector3(-1, -1, -1), Vector3(1, -1, -1)
	};

	for(unsigned int i = 0; i < 8; i++ )
	{
		const Vector3& corner = cornerPoints[i];

		Vector4 c = matInvClip * Vector4(corner, 1);
		corners[i++] = Vector3(c.x / c.w, c.y / c.w, c.z / c.w);
	}
}

//-----------------------------------//

IntersectionType Frustum::intersects(const BoundingBox& box) const
{
	unsigned int iTotalIn = 0;

	for(unsigned int p = 0; p < 6; ++p)
	{
		unsigned int iInCount = 8;
		unsigned int iPtIn = 1;

		for(unsigned int i = 0; i < 8; ++i)
		{
			if(planes[p].intersects(corners[i]) == IntersectionType::NegativeSide)
			{
				iPtIn = 0;
				--iInCount;
			}
		}

		if(iInCount == 0)
			return IntersectionType::Outside;

		iTotalIn += iPtIn;
	}

	if(iTotalIn == 6)
		IntersectionType::Inside;

	return IntersectionType::Intersects;
}

//-----------------------------------//

IntersectionType Frustum::intersects(const BoundingSphere& sphere) const
{
	for(unsigned int i = 0; i < 6; ++i)
	{
		float distance = planes[i].normal.dotProduct(sphere.center) + planes[i].offset;

		if(distance < -sphere.radius)
			return IntersectionType::Outside;

		if(Math::Abs(distance) < sphere.radius)
			return IntersectionType::Intersects;
	}

	return IntersectionType::Inside;
}

//-----------------------------------//