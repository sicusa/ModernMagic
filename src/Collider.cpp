#include "stdafx.h"

#include "Collider.h"
#include "Body.h"

MM_BEGIN

/****BoundingBoxCollider****/

BoundingBoxCollider::BoundingBoxCollider()
{
	this->InitProperties();
}

BoundingBoxCollider::BoundingBoxCollider(const BoundingBox &box):
	_box(box)
{
	this->InitProperties();
}

BoundingBoxCollider::BoundingBoxCollider(const BoundingBoxCollider &other):
	ICollider(other), _box(other._box)
{
	this->InitProperties();
}

void BoundingBoxCollider::InitProperties()
{
	this->RegisterProperty("BoundingBox", &_box);
}

bool BoundingBoxCollider::IsCollided(Body *body)
{
	MM_ASSERT(body);
	return body->IsCollided(_box);
}

/****RoundCollider****/

RoundCollider::RoundCollider()
{
	this->InitProperties();
}

RoundCollider::RoundCollider(const Vector2 &pos, float radius):
	_pos(pos), _radius(radius)
{
	this->InitProperties();
}

RoundCollider::RoundCollider(const RoundCollider &other):
	ICollider(other), _pos(other._pos), _radius(other._radius)
{
	this->InitProperties();
}

void RoundCollider::InitProperties()
{
	this->RegisterProperty("Position", &_pos);
	this->RegisterProperty("Radius", &_radius);
}

bool RoundCollider::IsCollided(Body *body)
{
	MM_ASSERT(body);
	return body->IsCollided(_pos, _radius);
}

/****SegmentCollider****/

SegmentCollider::SegmentCollider()
{
	this->InitProperties();
}

SegmentCollider::SegmentCollider(const Segment &seg):
	_seg(seg)
{
	this->InitProperties();
}

SegmentCollider::SegmentCollider(const SegmentCollider &other):
	ICollider(other), _seg(other._seg)
{
	this->InitProperties();
}

void SegmentCollider::InitProperties()
{
	this->RegisterProperty("Segment", &_seg);
}

bool SegmentCollider::IsCollided(Body *body)
{
	MM_ASSERT(body);
	return body->IsCollided(_seg);
}

MM_END