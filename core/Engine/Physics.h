#pragma once
#include "engineMath.h"

class CollisionBox;

class Physics
{
	public:
		class AABB
		{
			public:
				AABB() {};
				AABB(Vector3 min, Vector3 max) 
				{
					mMin = min; 
					mMax = max; 
				}

				Vector3 mMin;
				Vector3 mMax;
		};

		class LineSegment
		{
			public:
				Physics::LineSegment() {};
				Physics::LineSegment(Vector3 start, Vector3 end)
				{
					mStart = start;
					mEnd = end;
				}
				Vector3 mStart;
				Vector3 mEnd;
		};

		static bool Intersect(const AABB& a, const AABB& b, AABB* pOverlap = nullptr);
		static bool Intersect(const Physics::LineSegment& segment, const AABB& box, Vector3* pHitPoint = nullptr);
		static bool UnitTest();
		void AddCollisionBox(const CollisionBox* pObj);
		void RemoveCollisionBox(const CollisionBox* pObj);
		bool RayCast(const LineSegment& segment, Vector3* pHitPoint = nullptr);

		protected:
			std::vector<const CollisionBox*> mCollisionBoxes;
};

struct TestAABB
{
	Physics::AABB a;
	Physics::AABB b;
	Physics::AABB overlap;
};

const TestAABB testAABB[] =
{
	{
		Physics::AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(0.0f, 0.0f, 0.0f), Vector3(10.0f, 10.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-110.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(-100.0f, -10.0f, -10.0f), Vector3(-90.0f, 10.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(110.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3(90.0f, -10.0f, -10.0f), Vector3(100.0f, 10.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -110.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f)),
		Physics::AABB(Vector3(-10.0f, -100.0f, -10.0f), Vector3(10.0f, -90.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 110.0f, 10.0f)),
		Physics::AABB(Vector3(-10.0f, 90.0f, -10.0f), Vector3(10.0f, 100.0f, 10.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, -110.0f), Vector3(10.0f, 10.0f, -90.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, -100.0f), Vector3(10.0f, 10.0f, -90.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 110.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, 90.0f), Vector3(10.0f, 10.0f, 100.0f))
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-120.0f, -10.0f, -10.0f), Vector3(-110.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3::One, Vector3::Zero)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(110.0f, -10.0f, -10.0f), Vector3(120.0f, 10.0f, 10.0f)),
		Physics::AABB(Vector3::One, Vector3::Zero)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -120.0f, -10.0f), Vector3(10.0f, -110.0f, 10.0f)),
		Physics::AABB(Vector3::One, Vector3::Zero)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, 110.0f, -10.0f), Vector3(10.0f, 120.0f, 10.0f)),
		Physics::AABB(Vector3::One, Vector3::Zero)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, -120.0f), Vector3(10.0f, 10.0f, -110.0f)),
		Physics::AABB(Vector3::One, Vector3::Zero)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::AABB(Vector3(-10.0f, -10.0f, 110.0f), Vector3(10.0f, 10.0f, 120.0f)),
		Physics::AABB(Vector3::One, Vector3::Zero)
	}
};

struct TestSegment
{
	Physics::AABB box;
	Physics::Physics::LineSegment segment;
	bool hit;
	Vector3 point;
};

const TestSegment testSegment[] =
{
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(-110.0f, 0.0f, 0.0f), Vector3(-90.0f, 0.0f, 0.0f)),
		true, Vector3(-100.0f, 0.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, -110.0f, 0.0f), Vector3(0.0f, -90.0f, 0.0f)),
		true, Vector3(0.0f, -100.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, -110.0f), Vector3(0.0f, 0.0f, -90.0f)),
		true, Vector3(0.0f, 0.0f, -100.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(110.0f, 0.0f, 0.0f), Vector3(90.0f, 0.0f, 0.0f)),
		true, Vector3(100.0f, 0.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 110.0f, 0.0f), Vector3(0.0f, 90.0f, 0.0f)),
		true, Vector3(0.0f, 100.0f, 0.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, 110.0f), Vector3(0.0f, 0.0f, 90.0f)),
		true, Vector3(0.0f, 0.0f, 100.0f)
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(-120.0f, 0.0f, 0.0f), Vector3(-110.0f, 0.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, -120.0f, 0.0f), Vector3(0.0f, -110.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, -120.0f), Vector3(0.0f, 0.0f, -110.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(120.0f, 0.0f, 0.0f), Vector3(110.0f, 0.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 120.0f, 0.0f), Vector3(0.0f, 110.0f, 0.0f)),
		false, Vector3::Zero
	},
	{
		Physics::AABB(Vector3(-100.0f, -100.0f, -100.0f), Vector3(100.0f, 100.0f, 100.0f)),
		Physics::LineSegment(Vector3(0.0f, 0.0f, 120.0f), Vector3(0.0f, 0.0f, 110.0f)),
		false, Vector3::Zero
	}
};