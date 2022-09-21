#include "stdafx.h"
#include "Physics.h"
#include "CollisionBox.h"

bool Physics::Intersect(const AABB& a, const AABB& b, AABB* pOverlap)
{
	bool x_intersect = a.mMin.x <= b.mMax.x && a.mMax.x >= b.mMin.x;
	bool y_intersect = a.mMin.y <= b.mMax.y && a.mMax.y >= b.mMin.y;
	bool z_intersect = a.mMin.z <= b.mMax.z && a.mMax.z >= b.mMin.z;

	if (x_intersect && y_intersect && z_intersect)
	{
		pOverlap->mMax.x = Math::Min(a.mMax.x, b.mMax.x);
		pOverlap->mMin.x = Math::Max(a.mMin.x, b.mMin.x);
		pOverlap->mMax.y = Math::Min(a.mMax.y, b.mMax.y);
		pOverlap->mMin.y = Math::Max(a.mMin.y, b.mMin.y);
		pOverlap->mMax.z = Math::Min(a.mMax.z, b.mMax.z);
		pOverlap->mMin.z = Math::Max(a.mMin.z, b.mMin.z);

		return true;
	}

	return false;
}

bool Physics::Intersect(const LineSegment& segment, const AABB& box, Vector3* pHitPoint)
{
	float tmin = FLT_MIN;
	float tmax = FLT_MAX;

	// ray function R(t) = mStart + (mEnd - mStart) * t = p + d * t
	Vector3 p = segment.mStart;
	Vector3 d = segment.mEnd - segment.mStart;

	// X slab
	if ((p.x < box.mMin.x && segment.mEnd.x < box.mMin.x) || (p.x > box.mMax.x && segment.mEnd.x > box.mMax.x))
	{
		return false;
	}	
	else
	{
		float odd = 1.0f / d.x;
		float t1 = (box.mMin.x - p.x) * odd;
		float t2 = (box.mMax.x - p.x) * odd;

		if (t1 > t2)
		{
			std::swap(t1, t2);
		}

		tmin = Math::Max(tmin, t1);
		tmax = Math::Min(tmax, t2);

		if (tmin > tmax)
		{
			return false;
		}
	}

	// Y slab
	if ((p.y < box.mMin.y && segment.mEnd.y < box.mMin.y) || (p.y > box.mMax.y && segment.mEnd.y > box.mMax.y))
	{
		return false;
	}
	else
	{
		float odd = 1.0f / d.y;
		float t1 = (box.mMin.y - p.y) * odd;
		float t2 = (box.mMax.y - p.y) * odd;

		if (t1 > t2)
		{
			std::swap(t1, t2);
		}

		tmin = Math::Max(tmin, t1);
		tmax = Math::Min(tmax, t2);

		if (tmin > tmax)
		{
			return false;
		}
	}

	// Z slab
	if ((p.z < box.mMin.z && segment.mEnd.z < box.mMin.z) || (p.z > box.mMax.z && segment.mEnd.z > box.mMax.z))
	{
		return false;
	}
	else
	{
		float odd = 1.0f / d.z;
		float t1 = (box.mMin.z - p.z) * odd;
		float t2 = (box.mMax.z - p.z) * odd;

		if (t1 > t2)
		{
			std::swap(t1, t2);
		}

		tmin = Math::Max(tmin, t1);
		tmax = Math::Min(tmax, t2);

		if (tmin > tmax)
		{
			return false;
		}
	}

	// Point of intersection between line segment and AABB
	*pHitPoint = p + d * tmin;

	return true;
}

bool Physics::UnitTest()
{
	bool passAllTests = true;

	// Test the intersection between two AABBs
	for (size_t i = 0; i < sizeof(testAABB) / sizeof(testAABB[0]); i++)
	{
		AABB pOverlap;
		bool result = Physics::Intersect(testAABB[i].a, testAABB[i].b, &pOverlap);

		// Compare min and max of pOverlap with those of the correct one
		passAllTests = Vector3::IsCloseEnough(pOverlap.mMax, testAABB[i].overlap.mMax) &&
					   Vector3::IsCloseEnough(pOverlap.mMin, testAABB[i].overlap.mMin);
	}

	// Test the intersection between line segment and AABB
	for (size_t i = 0; i < sizeof(testSegment) / sizeof(testSegment[0]); i++)
	{
		Vector3 pHitPoint;
		bool result = Physics::Intersect(testSegment[i].segment, testSegment[i].box, &pHitPoint);

		// 1. Compare point of intersection of pHightPoint with that the correct one 2. Check 'hit'
		passAllTests = Vector3::IsCloseEnough(pHitPoint, testSegment[i].point) && 
					   (result == testSegment[i].hit);
	}

	return passAllTests;
}

void Physics::AddCollisionBox(const CollisionBox* pObj)
{
	mCollisionBoxes.push_back(pObj);
}

void Physics::RemoveCollisionBox(const CollisionBox* pObj)
{
	std::vector<const CollisionBox*>::iterator it = std::find(mCollisionBoxes.begin(), mCollisionBoxes.end(), pObj);

	if (it != mCollisionBoxes.end())
	{
		mCollisionBoxes.erase(it);
	}
}

bool Physics::RayCast(const LineSegment& segment, Vector3* pHitPoint)
{
	float minDistance = FLT_MAX;
	float didIntersect = false;

	Vector3 point;
	for (size_t i = 0; i < mCollisionBoxes.size(); i++)
	{
		if (Intersect(segment, mCollisionBoxes[i]->GetAABB(), &point))
		{
			didIntersect = true;
			if ((point - segment.mStart).LengthSq() < minDistance)
			{
				*pHitPoint = point;
			}
		}
	}

	return didIntersect;
}