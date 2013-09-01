#include "stdafx.h"

#include "Engine.h"

#include "Timeline/Timeline.h"

MM_BEGIN

Engine::Engine():
	_sb(new Spellbook())
{
	this->RegisterProperty("WorldBox", &_worldbox);
	this->RegisterProperty("MainTimeline", &MainTimeline());
	this->RegisterProperty("BodyUpdater", &BodyUpdater());
	this->RegisterProperty("ActionUpdater", &ActionUpdater());
}

Engine::~Engine()
{
	delete _sb;
}

void Engine::Update(float dt)
{
	BodyUpdater().Update(dt);
	ActionUpdater().Update(dt);

	if (MainTimeline().IsInstalled())
		MainTimeline().Update(dt);
}

void Engine::SetSpellbook(Spellbook *sb, bool isDestoryOld)
{
	MM_ASSERT(sb);
	if (isDestoryOld) {
		delete _sb;
	}
	_sb = sb;
}

Object *Engine::Clone() const
{
	throw UnimplementedClone("Engine[Clone]: 克隆 theMMEngine 是禁止的");
}

MM_END
