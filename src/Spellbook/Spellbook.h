#pragma once

#include "ObjectManager.h"

MM_BEGIN

class Spellbook
{
public:
	Spellbook();
	~Spellbook();

	/**
	 * 主时间轴
	 */
	Timeline &MainTimeline() {
		return *_timeline;
	}

	/**
	 * ActionUpdater
	 */
	ObjectUpdater<Action> &ActionUpdater() {
		return _actionUpdater;
	}

	/**
	 * ActionUpdater
	 */
	ObjectUpdater<Body>	&BodyUpdater() {
		return _bodyUpdater;
	}

private:
	ObjectUpdater<Action>	_actionUpdater;
	ObjectUpdater<Body>		_bodyUpdater;
	Timeline				*_timeline;
};

MM_END