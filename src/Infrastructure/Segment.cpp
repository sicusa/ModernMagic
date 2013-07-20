#include "stdafx.h"

#include "Segment.h"
#include "Vector3.h"
#include "BoundingShapes.h"

MM_BEGIN

/**
 * @brief 求矢量[p0, p1], [p0, p2]的叉积  
 * @param p1
 * @param p2
 * @param p0 顶点
 * @return 若结果等于0，则这三点共线。若结果大于0，则[p0, p2]在[p0, p1]的逆时针方向。若结果小于0，则[p0, p2]在[p0, p1]的顺时针方向  
*/
inline double _Multi(Vector2 p1, Vector2 p2, Vector2 p0)
{  
	return (p1.X - p0.X) * (p2.Y - p0.Y) - (p2.X - p0.X) * (p1.Y - p0.Y);  
}  
/**
 * @brief 判断两线段是否相交
 * @param s1 线段1起始点
 * @param e1 线段1结束点
 * @param s2 线段2起始点
 * @param e2 线段2结束点
 * return 两线段是否相交
*/
inline bool _IsIntersected(Vector2 s1, Vector2 e1, Vector2 s2, Vector2 e2)
{
	if((_Multi(s2, e1, s1) * _Multi(e1, e2, s1) >= 0) && 
		(_Multi(s1, e2, s2) * _Multi(e2, e1, s2) >= 0)) 
	{
		return true;
	}
	return false;
}

/*static*/ bool Segment::Intersects(const Vector2 &A, const Vector2 &B, const Vector2 &a, const Vector2 &b)
{
	 return _IsIntersected(A, B, a, b);
}

/*static*/ bool Segment::Intersects(const Vector2 &A, const Vector2 &B, const BoundingBox &box)
{
	if (box.Intersects(A) || box.Intersects(B)) {
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
	if (f < 0.f) return Vector2::Distance(point, A);

	float d = Vector2::Dot(ab, ab);
	if (f > d) return Vector2::Distance(point, B);

	f = f / d;

	Vector2 D = A + ab * f;
	return Vector2::Distance(point, D);
}

MM_END