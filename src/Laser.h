#pragma once

#include "Body.h"
#include "BodyTypeManager.h"
#include "Infrastructure/Segment.h"

MM_BEGIN

class Laser : public Body
{
	MM_TAG_OBJECT(Laser)
	MM_TAG_ENABLE_TYPE(Laser)
	
	MM_TAG_OVERRIDE_UPDATE
	
public:
	Laser();
	Laser(const Laser &other);
	
	/**
	 * 判断当前 Laser 是否与一个矩形发生碰撞
	 */
	virtual bool IsCollided(const BoundingBox &rect) const override;
	
	/**
	 * 判断当前 Laser 是否与一个圆形发生碰撞
	 */
	virtual bool IsCollided(const Vector2 &pos, float r) const override;
	
	/**
	 * 判断当前 Bullet 是否与线段发生碰撞
	 */
	virtual bool IsCollided(const Segment &seg) const override;
	
	/**
	 * 激光尾部
	 */
	MM_PROPERTY_PBR_NS(Vector2, _tail, Tail)
	
	/**
	 * 长度
	 */
	MM_PROPERTY_PBV(float, _length, Length)

	/**
	 * 已经展开的长度
	 */
	MM_PROPERTY_PBV_NS(float, _currLength, CurrentLength)
	
private:
	void InitProeprties();
};

MM_END
