#include "stdafx.h"

#include "Body.h"
#include "Timeline.h"
#include "Engine.h"

#include "Util/MathUtil.h"

#include <math.h>

MM_BEGIN

Body::Body():
	_texture(-1), _layer(0),
	_radius(0.0f), _angle(0.0f), _speed(0.0f),
	_angleAddition(0.0f), _speedAddition(0.0f),
	_isFrozen(false),
	_destroyWhenMoveOffScreen(true),
	_destroyWhenTimelineEnd(false),
	_appliedTimeline(nullptr),
	_isNeedToCal(false),
	_isReboundable(true), _opacity(1.0f)
{
	this->InitProperties();
}

Body::Body(const Body &other):
	UpdatableObject(other),
	_position(other._position),
	_texture(other._texture),
	_color(other._color), _layer(other._layer),
	_radius(other._radius), _angle(other._angle), _speed(other._speed),
	_angleAddition(other._angleAddition), _speedAddition(other._speedAddition),
	_isFrozen(other._isFrozen), 
	_destroyWhenMoveOffScreen(other._destroyWhenMoveOffScreen),
	_destroyWhenTimelineEnd(other._destroyWhenTimelineEnd),
	_appliedTimeline(nullptr),
	_isReboundable(other._isReboundable),
	_calculatedOffset(other._calculatedOffset),
	_isNeedToCal(other._isNeedToCal), _opacity(other._opacity)

{
	if (other._appliedTimeline) {
		_appliedTimeline = Object::Clone(other._appliedTimeline);
		_appliedTimeline->SetBindingObject(this);
	}
	this->InitProperties();
}

void Body::InitProperties()
{
	this->RegisterProperty("Position",  &_position);
	this->RegisterProperty("Texture",   &_texture);
	this->RegisterProperty("Color",     &_color);
	this->RegisterProperty("Layer",     &_layer);
	this->RegisterProperty("Radius",    &_radius);
	this->RegisterProperty("AngleAddition", &_angleAddition);
	this->RegisterProperty("SpeedAddition", &_speedAddition);
	this->RegisterProperty("IsFrozen",      &_isFrozen);
	this->RegisterProperty("IsDestroyWhenMoveOffScreen", &_destroyWhenMoveOffScreen);
	this->RegisterProperty("IsDestroyWhenTimelineEnd", &_destroyWhenTimelineEnd);

	this->RegisterProperty("IsReboundable", &_isReboundable,
						   PropertyPermission::Readonly);
	this->RegisterProperty("CalculatedOffset", &_calculatedOffset,
						   PropertyPermission::Readonly);

	this->RegisterProperty<float> ("Opacity", &_opacity);
	this->RegisterProperty<float> (
		"Angle",
		std::bind(&Body::SetAngle, this, std::placeholders::_1),
		std::bind(&Body::GetAngle, this)
	);
	this->RegisterProperty<float> (
		"Speed",
		std::bind(&Body::SetSpeed, this, std::placeholders::_1),
		std::bind(&Body::GetSpeed, this)
	);
	this->RegisterProperty<Timeline*> (
		"AppliedTimeline",
		std::bind(&Body::SetAppliedTimeline, this, std::placeholders::_1),
		std::bind(&Body::GetAppliedTimeline, this)
	);
}

Body::~Body()
{
	if (_appliedTimeline != nullptr)
		_appliedTimeline->Destroy();
}

Timeline *Body::AppliedTimeline()
{
	if (_appliedTimeline == nullptr) {
		this->SetAppliedTimeline(new Timeline());
	}
	return _appliedTimeline;
}

void Body::SetAppliedTimeline(Timeline *v)
{
	_appliedTimeline = v;

	if (v != nullptr) {
		_appliedTimeline->SetBindingObject(this);
	}
}

void Body::SetSpeed(float v)
{
	_speed = v;
	_isNeedToCal = true;
}

void Body::SetAngle(float v)
{
	_angle = v;
	while (_angle >= 360.0f) {
		_angle -= 360.0f;
	}
	_isNeedToCal = true;
}

void Body::Calculate()
{
	float tmp = _angle * MathUtil::Pi / 180.0f;
	_calculatedOffset.X = _speed * sin(tmp);
	_calculatedOffset.Y = -_speed * cos(tmp);
}

bool Body::OnInstalling()
{
	if (_appliedTimeline) {
		if (!_appliedTimeline->Install()) {
			return false;
		}
	}
	return UpdatableObject::OnInstalling();
}

bool Body::Update(float dt)
{
	if (_appliedTimeline && _appliedTimeline->IsInstalled() &&
		_appliedTimeline->Update(dt))
	{
		_appliedTimeline->Uninstall();
		if (_destroyWhenTimelineEnd) {
			return true;
		}
	}

	if (_isFrozen)
		return false;

	if (_destroyWhenMoveOffScreen &&
		!this->IsCollided(theMMEngine.GetWorldBox())) {
			return true;
	}

	if (_angleAddition || _speedAddition)
	{
		_angle += _angleAddition * dt;
		_speed += _speedAddition * dt;
		this->Calculate();
	}
	else if (_isNeedToCal)
	{
		this->Calculate();
		_isNeedToCal = false;
	}

	_position += _calculatedOffset * dt;
	return false;
}

MM_END
