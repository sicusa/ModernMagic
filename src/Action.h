#pragma once

#include "Object.h"

MM_BEGIN

class Action : public UpdatableObject
{
	MM_TAG_ABSTRACT(Action);

public:
	Action();
	Action(const Action &other);

	// 获取根时间轴绑定的 Object
	Object *GetRootObject();

public:
	// 绑定的根时间轴
	MM_PROPERTY_PBV_NS(Timeline*, _rootTimeline, RootTimeline)
	virtual void SetRootTimeline(Timeline *tl) { _rootTimeline = tl; }

	// 其动作的父动作
	// 如果此动作处于根时间轴的下一级，则父动作就是 Root Timeline
	MM_PROPERTY_PBV(Action*, _parent, Parent)

private:
	void InitProperties();
};

MM_END
