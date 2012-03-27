#ifndef __Frustum_h__
#define __Frustum_h__

#include "scge\Math.h"

#include "scge\Math\Matrix4.h"
#include "scge\Math\Plane.h"
#include "scge\Math\Vector3.h"

#include <array>

//-----------------------------------//

enum class ProjectionType
{
	Orthographic,
	Perspective
};

//-----------------------------------//

class Frustum
{
public:
	Frustum();
	Frustum(const Frustum& rhs);

	void updateProjection(const Vector2& size);
	void updatePlanes(const Matrix4& matView);
	void updateCorners(const Matrix4& matView);

	IntersectionType intersects(const BoundingBox& box) const;
	IntersectionType intersects(const BoundingSphere& sphere) const;

	ProjectionType projection;
	Matrix4 matProjection;

	float fieldOfView;
	float aspectRatio;

	float nearPlane;
	float farPlane;
	
	std::array<Plane, 6> planes;
	std::array<Vector3, 8> corners;
};

//-----------------------------------//

#endif // __Frustum_h__