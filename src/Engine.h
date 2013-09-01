#pragma once

#include "Spellbook/Spellbook.h"
#include "Infrastructure/BoundingShapes.h"

#define theMMEngine			::ModernMagic::Engine::GetInstance()
#define theMMMainTimeline	theMMEngine.MainTimeline()
#define theMMBodyUpdater	theMMEngine.BodyUpdater()
#define theMMActionUpdater	theMMEngine.ActionUpdater()
#define theMMNamedObjects	theMMEngine.NamedObjects()

MM_BEGIN

class MovableObject;
class Action;

class Engine : public Object
{
	MM_TAG_DESTROYABLE

public:
	static Engine &GetInstance() {
		static Engine e;
		return e;
	}

	~Engine();
	
	MM_PROPERTY_PBR(BoundingBox, _worldbox,	WorldBox)

	Timeline &MainTimeline() {
		return _sb->MainTimeline();
	}
	
	ObjectUpdater<Body> &BodyUpdater() {
		return _sb->BodyUpdater();
	}
	
	ObjectUpdater<Action> &ActionUpdater() {
		return _sb->ActionUpdater();
	}

	/**
	 * 获取所有的被命名的对象
	 */
	std::unordered_map<String, Object*> &NamedObjects() {
		return _namedObjects;
	}

	void SetSpellbook(Spellbook *sb, bool isDestoryOld = true);
	void Update(float dt);

	virtual Object *Clone() override {
		return static_cast<const Object*>(this)->Clone();
	}
	virtual Object *Clone() const override;

private:
	Engine();

	Spellbook *_sb;
	std::unordered_map<String, Object*> _namedObjects;
};

MM_END
