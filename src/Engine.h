#pragma once

#include "ObjectManager.h"
#include "Timeline.h"

#include "Infrastructure/BoundingShapes.h"

#define theMMEngine ::ModernMagic::Engine::GetInstance()
#define theMMMainTimeline  theMMEngine.MainTimeline()
#define theMMBodyUpdater   theMMEngine.BodyUpdater()
#define theMMActionUpdater theMMEngine.ActionUpdater()

MM_BEGIN

class MovableObject;
class Action;

class Engine : public Object
{
public:
	static Engine &GetInstance()
	{
		static Engine e;
		return e;
	}
	
	MM_PROPERTY_PBR(BoundingBox, _worldbox,	WorldBox)

	Timeline &MainTimeline() {
		return _mainTimeline;
	}
	
	ObjectUpdater<Body> &BodyUpdater() {
		return _bodyUpdater;
	}
	
	ObjectUpdater<Action> &ActionUpdater() {
		return _actionUpdater;
	}
	
	bool Start();
	
	void Update(float dt);

	virtual Object *Clone() override {
		return static_cast<const Object*>(this)->Clone();
	}
	virtual Object *Clone() const override;
	virtual void Destroy() override;

protected:
	Engine();
	
private:
	Timeline                _mainTimeline;
	ObjectUpdater<Body>     _bodyUpdater;
	ObjectUpdater<Action>   _actionUpdater;
};

MM_END
