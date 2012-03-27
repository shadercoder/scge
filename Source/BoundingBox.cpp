#include "scge\Math\BoundingBox.h"

#include "scge\Math\Ray.h"
#include "scge\Math\Matrix4.h"

#include "scge\Warning.h"

//-----------------------------------//

BoundingBox::BoundingBox()
	: min()
	, max()
{ }

//-----------------------------------//

BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
	: min(min)
	, max(max)
{ }

//-----------------------------------//

BoundingBox::BoundingBox(const BoundingBox& box)
	: min(box.min)
	, max(box.max)
{ }

//-----------------------------------//

void BoundingBox::setZero()
{
	min = 0;
	max = 0;
}

//-----------------------------------//

bool BoundingBox::isInfinite() const
{
	return min == Math::Limits::FloatMaximum && max == Math::Limits::FloatMinimum;
}

//-----------------------------------//

void BoundingBox::reset()
{
	min = Math::Limits::FloatMaximum;
	max = Math::Limits::FloatMinimum;
}

//-----------------------------------//

void BoundingBox::add(const Vector3& v)
{
	min.x = std::min(min.x, v.x);
	min.y = std::min(min.y, v.y);
	min.z = std::min(min.z, v.z);

	max.x = std::max(max.x, v.x);
	max.y = std::max(max.y, v.y);
	max.z = std::max(max.z, v.z);
}

//-----------------------------------//

void BoundingBox::add(const BoundingBox& box)
{
	min.x = std::min(min.x, box.min.x);
	min.y = std::min(min.y, box.min.y);
	min.z = std::min(min.z, box.min.z);

	max.x = std::max(max.x, box.max.x);
	max.y = std::max(max.y, box.max.y);
	max.z = std::max(max.z, box.max.z);
}

//-----------------------------------//

BoundingBox BoundingBox::transform(const Matrix4& mat) const
{
	BoundingBox box;
	box.reset();

	for(int i = 0; i < 8; i++)
	{
		Vector3 point = mat * getCorner(i);
		box.add(point);
	}

	return box;
}

//-----------------------------------//

Vector3 BoundingBox::getCorner(int i) const
{
	SCGE_ASSERT(i >= 0 && i <= 8);
	return Vector3(
		(i & 1) ? max.x : min.x,
		(i & 2) ? max.y : min.y,
		(i & 4) ? max.z : min.z);
}

//-----------------------------------//

Vector3 BoundingBox::getCenter() const
{
	return (min + max) * 0.5f;
}

//-----------------------------------//

bool BoundingBox::intersects(const Ray& ray, float& distance) const
{
	float lowt = 0.0f;
	float t;

	bool hit = false;
	Vector3 hitpoint;

	// Check origin inside first
	if ( ray.origin > min && ray.origin < max )
	{
		distance = 0;
		return true;
	}

	// Check each face in turn, only check closest 3
	// Min x
	if (ray.origin.x <= min.x && ray.direction.x > 0)
	{
		t = (min.x - ray.origin.x) / ray.direction.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = ray.origin + ray.direction * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max x
	if (ray.origin.x >= max.x && ray.direction.x < 0)
	{
		t = (max.x - ray.origin.x) / ray.direction.x;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = ray.origin + ray.direction * t;
			if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min y
	if (ray.origin.y <= min.y && ray.direction.y > 0)
	{
		t = (min.y - ray.origin.y) / ray.direction.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = ray.origin + ray.direction * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max y
	if (ray.origin.y >= max.y && ray.direction.y < 0)
	{
		t = (max.y - ray.origin.y) / ray.direction.y;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = ray.origin + ray.direction * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.z >= min.z && hitpoint.z <= max.z &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Min z
	if (ray.origin.z <= min.z && ray.direction.z > 0)
	{
		t = (min.z - ray.origin.z) / ray.direction.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = ray.origin + ray.direction * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}
	// Max z
	if (ray.origin.z >= max.z && ray.direction.z < 0)
	{
		t = (max.z - ray.origin.z) / ray.direction.z;
		if (t >= 0)
		{
			// Substitute t back into ray and check bounds and dist
			hitpoint = ray.origin + ray.direction * t;
			if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
				hitpoint.y >= min.y && hitpoint.y <= max.y &&
				(!hit || t < lowt))
			{
				hit = true;
				lowt = t;
			}
		}
	}

	distance = lowt;
	return hit;
}

//-----------------------------------//