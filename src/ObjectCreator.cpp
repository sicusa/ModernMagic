#include "stdafx.h"

#include "ObjectCreator.h"

#include "Timeline/Timeline.h"
#include "Timeline/Components.h"
#include "Infrastructure/Vector2.h"
#include "Infrastructure/Vector3.h"

#include "Laser.h"
#include "Bullet.h"

MM_BEGIN

ObjectCreator::ObjectCreator()
{
	this->Register<Bullet>("Bullet");
	this->Register<Laser>("Laser");

	// Actions
	this->Register<Timeline>("Timeline");
	this->Register<LoopTimeline>("LoopTimeline");
	this->Register<ActionGroup>("ActionGroup");
	this->Register<TimingActionGroup>("TimingActionGroup");
	this->Register<ActionSleep>("Sleep");
	this->Register<ActionCallback>("Callback");
	this->Register<ActionRandom>("Random");
	this->Register<MessageSender>("MessageSender");
	this->Register<BodyEmitter>("Emitter");
	this->Register<AnnularEmitter>("AnnularEmitter");
	this->Register<CallbackSelector>("CallbackSelector");
	this->Register<CollisionSelector>("CollisionSelector");
	this->Register<ReboundBoard>("ReboundBoard");

	this->Register<ChangeTo<float>>("ChangeFloatTo");
	this->Register<ChangeBy<float>>("ChangeFloatBy");
	this->Register<ChangeTo<bool>>("ChangeBoolTo");
	this->Register<ChangeBy<bool>>("ChangeBoolBy");
	this->Register<ChangeTo<Vector2>>("ChangeVector2To");
	this->Register<ChangeBy<Vector2>>("ChangeVector2By");
	this->Register<ChangeTo<Vector3>>("ChangeVector3To");
	this->Register<ChangeBy<Vector3>>("ChangeVector3By");
	this->Register<ChangeTo<size_t>>("ChangeUIntTo");
	this->Register<ChangeBy<size_t>>("ChangeUIntBy");
	this->Register<ChangeTo<int>>("ChangeIntTo");
	this->Register<ChangeBy<int>>("ChangeIntBy");

	this->Register<CallbackAnimation<float>>("CallbackFloatAnimation");
	this->Register<CallbackAnimation<Vector2>>("CallbackVector2Animation");
	this->Register<CallbackAnimation<Vector3>>("CallbackVector3Animation");
	this->Register<CallbackAnimation<size_t>>("CallbackUIntAnimation");
	this->Register<CallbackAnimation<int>>("CallbackIntAnimation");

	this->Register<AnimateTo<float>>("FloatAnimateTo");
	this->Register<AnimateBy<float>>("FloatAnimateBy");
	this->Register<AnimateTo<Vector2>>("Vector2AnimateTo");
	this->Register<AnimateBy<Vector2>>("Vector2AnimateBy");
	this->Register<AnimateTo<Vector3>>("Vector3AnimateTo");
	this->Register<AnimateBy<Vector3>>("Vector3AnimateBy");
	this->Register<AnimateTo<size_t>>("UIntAnimateTo");
	this->Register<AnimateBy<size_t>>("UIntAnimateBy");
	this->Register<AnimateTo<int>>("IntAnimateTo");
	this->Register<AnimateBy<int>>("IntAnimateBy");
}

void ObjectCreator::Register(const String &name, const CreatorFunc &creator)
{
	_creators[name] = creator;
}

Object *ObjectCreator::Create(const String &className)
{
	auto iter = _creators.find(className);
	if (iter == _creators.end()) {
		MM_THROW ClassFactoryCreateObjectFailed(
			"ClassFactory[CreateObject]: 对象创建失败，未注册的类型名 '"+
			className + '\''
		);
		return nullptr;
	}
	return iter->second();
}

MM_END