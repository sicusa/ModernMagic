#include "stdafx.h"

#include "Spellbook.h"
#include "../Timeline/Timeline.h"

MM_BEGIN

Spellbook::Spellbook():
	_timeline(new Timeline())
{
}

Spellbook::~Spellbook()
{
	if (_timeline) {
		_timeline->Destroy();
	}
}

MM_END