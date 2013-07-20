#include "stdafx.h"

#include "Segment.h"
#include "Vector3.h"
#include "BoundingShapes.h"

MM_BEGIN

inline double DirectionV3(double x1, double y1, double x2, double y2, double x3, double y3)
{
	return x1 * y3 + x2 * y1 + x3 * y2 - x1 * y2 - x2 * y3 - x3 * y1;
}

inline bool PointIntersects(const Vector2 &point, const BoundingBox &box)
{
	bool blResult = (point.X >= box.Min.X && point.X <= box.Max.X) &&
		(point.Y >= box.Min.Y && point.Y <= box.Max.Y);
	return blResult;
}

/*static*/ bool Segment::Intersects(const Vector2 &A, const Vector2 &B, const Vector2 &a, const Vector2 &b)
{
	double x1 = A.X, y1 = A.Y;
	double x2 = B.X, y2 = B.Y;
	double x3 = a.X, y3 = a.Y;
	double x4 = b.X, y4 = b.Y;

	if (DirectionV3(x1, y1, x2, y2, x3, y3) *
		DirectionV3(x1, y1, x2, y2, x4, y4) > 0)
		return false;

	if (DirectionV3(x3, y3, x4, y4, x1, y1) *
		DirectionV3(x3, y3, x4, y4, x2, y2) > 0)
		return false;

	return true;
}

/*static*/ bool Segment::Intersects(const Vector2 &A, const Vector2 &B, const BoundingBox &box)
{
	if (PointIntersects(A, box) || PointIntersects(B, box) ) {
		return true;
	}

	Vector2 upperRight(box.Max.X, box.Min.Y);
	Vector2 bottomLeft(box.Min.X, box.Max.Y);

	if (Intersects(A, B, box.Min, upperRight) ||
		Intersects(A, B, upperRight, box.Max) ||
		Intersects(A, B, box.Max, bottomLeft) ||
		Intersects(A, B, bottomLeft, box.Min))
	{
		return true;
	}

	return false;
}

/*static*/ float Segment::Distance(const Vector2 &A, const Vector2 &B, const Vector2 &point)
{
	Vector2 ab = B - A;
	Vector2 ac = point - A;

	float f = Vector2::Dot(ab, ac);
	if (f < 0) return Vector2::Distance(point, A);

	float d = Vector2::Dot(ab, ab);
	if (f > d) return Vector2::Distance(point, B);

	f = f / d;

	Vector2 D = A + ab * f;
	return Vector2::Distance(point, D);
}

MM_END