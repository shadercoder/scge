#ifndef __Frustum_h__
#define __Frustum_h__

#include "scge\Math.h"

#include "scge\Math\Matrix4.h"

//-----------------------------------//

/**
 * Projection types.
 */
enum class ProjectionType
{
	Orthographic,
	Perspective
};

//-----------------------------------//

/**
 * Frustum is a shape similiar to a pyramid with the top truncated.
 */
class Frustum
{
public:

	Frustum();
	Frustum(const Frustum& rhs);

	// Updates the projection matrix.
	void updateProjection(const Vector2& size);

	// Updates the frustum planes.
	void updatePlanes(const Matrix4& matView);

	// Updates the frustum corners.
	void updateCorners(const Matrix4& matView);

	// Checks if the box is inside the frustum.
	IntersectionType intersects(const BoundingBox& box) const;

	// Checks if the box is inside the frustum.
	IntersectionType intersects(const BoundingSphere& sphere) const;

public:

	// Projection type.
	ProjectionType projection;
	
	// Projection matrix.
	Matrix4 matProjection;

	// Field of view.
	float fieldOfView;

	// Near clipping plane.
	float nearPlane;
	
	// Far clipping plane.
	float farPlane;

	// Aspect ratio of the frustum.
	float aspectRatio;

	// Stores the planes that make up the frustum.
	// Order: Left, Right, Top, Bottom, Near, Far.
	Plane planes[6];

	// Frustum corner points.
	Vector3 corners[8];
};

//-----------------------------------//

#endif // __Frustum_h__