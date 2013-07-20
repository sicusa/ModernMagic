#include "stdafx.h"

#include "Action.h"
#include "Timeline.h"

MM_BEGIN

Action::Action():
	_rootTimeline(nullptr),
	_parent(nullptr)
{
	this->InitProperties();
}

Action::Action(const Action &other):
	UpdatableObject(other),
	_rootTimeline(other._rootTimeline),
	_parent(other._parent)
{
	this->InitProperties();
}

Action::~Action()
{
}

void Action::InitProperties()
{
	this->RegisterProperty<Timeline*> (
		"RootTimeline",
		std::bind(&Action::SetRootTimeline, this, std::placeholders::_1),
		std::bind(&Action::GetRootTimeline, this)
	);
	this->RegisterProperty("Parent", &_parent);
}

Object *Action::GetRootBindedObject()
{
	return _rootTimeline ? _rootTimeline->GetBindedObject() : nullptr;
}

MM_END
