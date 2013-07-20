#include "stdafx.h"

#include "Laser.h"

MM_BEGIN

Laser::Laser():
	_type(nullptr),
	_length(0.0f), _currLength(0.0f)
{
	this->SetIsReboundable(false);
	this->InitProeprties();
}

Laser::Laser(const Laser &other):
	Body(other),
	_type(other._type), _tail(other._tail), _length(other._length),
	_currLength(other._currLength)
{
	this->InitProeprties();
}

bool Laser::IsCollided(const BoundingBox &rect) const
{
	return Segment::Intersects(this->GetPosition(), this->GetTail(), rect);
}

void Laser::InitProeprties()
{
	this->RegisterProperty("Length", &_length);
	this->RegisterProperty("CurrentLength", &_currLength,
						   PropertyPermission::Readonly);
	this->RegisterProperty("Tail", &_tail,
						   PropertyPermission::Readonly);
	this->RegisterPropertySetter<String> (
		"Type",
		std::bind(&Laser::SetType, this, std::placeholders::_1)
	);
}

bool Laser::IsCollided(const Vector2 &pos, float r) const
{
	return Segment::Distance(this->GetPosition(), this->GetTail(), pos) < r + this->GetRadius();
}

bool Laser::IsCollided(const Segment &seg) const
{
	return Segment::Intersects(this->GetPosition(), this->GetTail(), seg.A, seg.B);
}

MM_IMPL_UPDATE(Laser, dt)
{
	if (Body::Update(dt))
		return true;
	
	if (_currLength < _length) {
		_currLength += this->GetSpeed() * dt;
		
	} else {
		_tail += this->GetCalculatedOffset() * dt;
	}
	return false;
}

MM_END
