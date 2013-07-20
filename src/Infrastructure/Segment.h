#pragma once

#include "Vector2.h"
#include "BoundingShapes.h"

MM_BEGIN

struct Segment
{
	// 线段两头的端点
	Vector2 A, B;

	Segment()
	{
	}

	Segment(const Vector2 &a, const Vector2 &b):
		A(a), B(b)
	{
	}

	Segment(float ax, float ay, float bx, float by):
		A(ax, ay), B(bx, by)
	{
	}

	Segment(const Segment &other):
		A(other.A), B(other.B)
	{
	}

	const Segment &operator=(const Segment &other) {
		A = other.A;
		B = other.B;
		return *this;
	}

	void DebugDraw() const;

	float Length() const {
		return Vector2::Distance(A, B);
	}

	float LengthSquared() const {
		return Vector2::DistanceSquared(A, B);
	}

	void Rotate(const float radians) {
		B = Vector2::Rotate(B, radians);
	}

	// 线段是否与圆相交
	static bool Intersects(const Vector2 &A, const Vector2 &B, const Vector2 &point, float radius) {
		return Distance(A, B, point) < radius;
	}
	bool Intersects(const Vector2 &point, float radius) const {
		return Distance(*this, point) < radius;
	}

	// 线段是否与另一条线段相交
	static bool Intersects(const Vector2 &A, const Vector2 &B, const Vector2 &a, const Vector2 &b);
	bool Intersects(const Segment &seg) const {
		return Intersects(A, B, seg.A, seg.B);
	}

	// 线段是否与矩形相交
	static bool Intersects(const Vector2 &A, const Vector2 &B, const BoundingBox &box);
	bool Intersects(const BoundingBox &box) const {
		return Intersects(A, B, box);
	}

	static float Distance(const Vector2 &A, const Vector2 &B, const Vector2 &point);

	static float Distance(const Segment &seg, const Vector2 &point) {
		return Distance(seg.A, seg.B, point);
	}

	static Segment Rotate(const Segment& value, const float radians)
	{
		Segment seg(value);
		seg.Rotate(radians);
		return seg;
	}
};

MM_END