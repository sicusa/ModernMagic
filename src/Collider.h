#pragma once

#include "Object.h"

#include "Infrastructure/Vector2.h"
#include "Infrastructure/BoundingShapes.h"
#include "Infrastructure/Segment.h"

MM_BEGIN

class ICollider : public Object
{
public:
	virtual ~ICollider() {}

	virtual bool IsCollided(Body *body) = 0;
};

class BoundingBoxCollider : public ICollider
{
	MM_TAG_OBJECT(BoundingBoxCollider)

public:
	BoundingBoxCollider();
	BoundingBoxCollider(const BoundingBox &box);
	BoundingBoxCollider(const BoundingBoxCollider &other);

	virtual bool IsCollided(Body *body) override;

public:
	MM_PROPERTY_PBR(BoundingBox, _box, BoundingBox)
	
private:
	void InitProperties();
};

class RoundCollider : public ICollider
{
	MM_TAG_OBJECT(RoundCollider)

public:
	RoundCollider();
	RoundCollider(const Vector2 &pos, float radius);
	RoundCollider(const RoundCollider &other);

	virtual bool IsCollided(Body *body) override;

public:
	MM_PROPERTY_PBV(Vector2, _pos, Position)
	MM_PROPERTY_PBV(float, _radius, Radius)
	
private:
	void InitProperties();
};

class SegmentCollider : public ICollider
{
	MM_TAG_OBJECT(SegmentCollider)

public:
	SegmentCollider();
	SegmentCollider(const Segment &seg);
	SegmentCollider(const SegmentCollider &other);

	virtual bool IsCollided(Body *body) override;

public:
	MM_PROPERTY_PBR(Segment, _seg, Segment)
	
private:
	void InitProperties();
};

MM_END