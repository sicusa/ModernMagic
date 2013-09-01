#pragma once

#include "Body.h"
#include "BodyTypeManager.h"

#include "Infrastructure/MemoryPool.h"

MM_BEGIN

class ReboundBoard;

class Bullet : public Body
{
	MM_TAG_CLONEABLE_UNIMPL
	MM_TAG_DESTROYABLE_UNIMPL
	MM_TAG_ENABLE_TYPE(Bullet)
	
public:
	friend class Engine;

	Bullet();
	Bullet(const Bullet &other);
	
	/**
	 * 判断当前 Bullet 是否与另一个发生碰撞
	 */
	bool IsCollided(Bullet *other) const;
	
	/**
	 * 判断当前 Bullet 是否与一个激光发生碰撞
	 */
	bool IsCollided(Laser *other) const;
	
	/**
	 * 判断当前 Bullet 是否与一个矩形发生碰撞
	 */
	virtual bool IsCollided(const BoundingBox &rect) const override;
	
	/**
	 * 判断当前 Bullet 是否与一个圆形发生碰撞
	 */
	virtual bool IsCollided(const Vector2 &pos, float r) const override;
	
	/**
	 * 判断当前 Bullet 是否与线段发生碰撞
	 */
	virtual bool IsCollided(const Segment &seg) const override;
	
	/**
	 * 子弹碰撞到的反弹板
	 * 不需要手动修改
	 */
	MM_PROPERTY_PBV(ReboundBoard*, _colReBoard, CollidedReboundBoard)

private:
	void InitProperties();
};

#define theMMBulletPool MemoryPool<Bullet>::GetInstance()

MM_END
