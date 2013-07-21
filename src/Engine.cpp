#include "stdafx.h"

#include "ModernMagic.h"

MM_BEGIN

Engine::Engine()
{
	this->RegisterProperty("WorldBox", &_worldbox);
	this->RegisterProperty("MainTimeline", &_mainTimeline);
	this->RegisterProperty("BodyUpdater", &_bodyUpdater);
	this->RegisterProperty("ActionUpdater", &_actionUpdater);

	theMMClassFactory.Register<Bullet>("Bullet");
	theMMClassFactory.Register<Laser>("Laser");

	// Actions
	theMMClassFactory.Register<Timeline>("Timeline");
	theMMClassFactory.Register<LoopTimeline>("LoopTimeline");
	theMMClassFactory.Register<ActionGroup>("ActionGroup");
	theMMClassFactory.Register<TimingActionGroup>("TimingActionGroup");
	theMMClassFactory.Register<ActionSleep>("Sleep");
	theMMClassFactory.Register<ActionCallback>("Callback");
	theMMClassFactory.Register<ActionRandom>("Random");
	theMMClassFactory.Register<MessageSender>("MessageSender");
	theMMClassFactory.Register<BodyEmitter>("Emitter");
	theMMClassFactory.Register<AnnularEmitter>("AnnularEmitter");
	theMMClassFactory.Register<CallbackSelector>("CallbackSelector");
	theMMClassFactory.Register<CollisionSelector>("CollisionSelector");
	theMMClassFactory.Register<ReboundBoard>("ReboundBoard");

	theMMClassFactory.Register<ChangeTo<float>>("ChangeFloatTo");
	theMMClassFactory.Register<ChangeBy<float>>("ChangeFloatBy");
	theMMClassFactory.Register<ChangeTo<bool>>("ChangeBoolTo");
	theMMClassFactory.Register<ChangeBy<bool>>("ChangeBoolBy");
	theMMClassFactory.Register<ChangeTo<Vector2>>("ChangeVector2To");
	theMMClassFactory.Register<ChangeBy<Vector2>>("ChangeVector2By");
	theMMClassFactory.Register<ChangeTo<Vector3>>("ChangeVector3To");
	theMMClassFactory.Register<ChangeBy<Vector3>>("ChangeVector3By");
	theMMClassFactory.Register<ChangeTo<size_t>>("ChangeUIntTo");
	theMMClassFactory.Register<ChangeBy<size_t>>("ChangeUIntBy");
	theMMClassFactory.Register<ChangeTo<int>>("ChangeIntTo");
	theMMClassFactory.Register<ChangeBy<int>>("ChangeIntBy");

	theMMClassFactory.Register<CallbackAnimation<float>>("CallbackFloatAnimation");
	theMMClassFactory.Register<CallbackAnimation<bool>>("CallbackBoolAnimation");
	theMMClassFactory.Register<CallbackAnimation<Vector2>>("CallbackVector2Animation");
	theMMClassFactory.Register<CallbackAnimation<Vector3>>("CallbackVector3Animation");
	theMMClassFactory.Register<CallbackAnimation<size_t>>("CallbackUIntAnimation");
	theMMClassFactory.Register<CallbackAnimation<int>>("CallbackIntAnimation");

	theMMClassFactory.Register<AnimateTo<float>>("FloatAnimateTo");
	theMMClassFactory.Register<AnimateBy<float>>("FloatAnimateBy");
	theMMClassFactory.Register<AnimateTo<bool>>("BoolAnimateTo");
	theMMClassFactory.Register<AnimateBy<bool>>("BoolAnimateBy");
	theMMClassFactory.Register<AnimateTo<Vector2>>("Vector2AnimateTo");
	theMMClassFactory.Register<AnimateBy<Vector2>>("Vector2AnimateBy");
	theMMClassFactory.Register<AnimateTo<Vector3>>("Vector3AnimateTo");
	theMMClassFactory.Register<AnimateBy<Vector3>>("Vector3AnimateBy");
	theMMClassFactory.Register<AnimateTo<size_t>>("UIntAnimateTo");
	theMMClassFactory.Register<AnimateBy<size_t>>("UIntAnimateBy");
	theMMClassFactory.Register<AnimateTo<int>>("IntAnimateTo");
	theMMClassFactory.Register<AnimateBy<int>>("IntAnimateBy");
}

void Engine::Update(float dt)
{
	_bodyUpdater.Update(dt);
	_actionUpdater.Update(dt);

	if (_mainTimeline.IsInstalled())
		_mainTimeline.Update(dt);
}

Object *Engine::Clone() const
{
	throw UnimplementedCloneException("You cannot clone theMMEngine");
}

void Engine::Destroy()
{
	throw UnimplementedDestroyException("You cannot destory theMMEngine");
}

MM_END
