#include "stdafx.h"

#include "Components.h"

#include "../Util/MathUtil.h"
#include "Messaging/Switchboard.h"

#include "../Engine.h"
#include "../Body.h"
#include "../Bullet.h"
#include "../Collider.h"

MM_BEGIN


/****ActionGroup****/

ActionGroup::ActionGroup():
	_uobj_num(0)
{
}

bool ActionGroup::OnAdd(Object *obj)
{
	MM_ASSERT(obj);
	auto ptr = static_cast<Action*>(obj);
	
	ptr->SetRootTimeline(this->GetRootTimeline());
	ptr->SetParent(this);
	return true;
}

void ActionGroup::OnRemove(Object *obj)
{
	MM_ASSERT(obj);
	auto ptr = static_cast<Action*>(obj);
	
	if (!ptr->IsInstalled() && _uobj_num) {
		--_uobj_num;
	}
	ObjectManager::OnRemove(obj);
}

bool ActionGroup::OnIterate(Object *obj, float dt)
{
	MM_ASSERT(obj);
	auto ptr = static_cast<Action*>(obj);
	
	if (ptr->IsInstalled() && ptr->Update(dt)) {
		ptr->Uninstall();
		++_uobj_num;
	}
	return false;
}

bool ActionGroup::OnInstalling()
{
	if (!ObjectManager::OnInstalling()) {
		return false;
	}
	
	for (auto each : this->GetObjects()) {
		try {
			if (!static_cast<Action*>(each)->Install()) {
				++_uobj_num;
			}
		}
		catch (InstallingFailed &e) {
			MM_THROW InstallingFailed("ActionGroup[OnInstalling]: 子动作安装失败 -->\n    " + e.str());
		}
	}
	return true;
}

void ActionGroup::OnUninstalled()
{
	ObjectManager::OnUninstalled();
	
	for (auto each : this->GetObjects()) {
		static_cast<Action*>(each)->Uninstall();
	}
	_uobj_num = 0;
}

void ActionGroup::SetRootTimeline(Timeline *tl)
{
	ObjectManager::SetRootTimeline(tl);
	
	for (auto each : this->GetObjects())
		static_cast<Action*>(each)->SetRootTimeline(tl);
}

MM_IMPL_UPDATE(ActionGroup, dt)
{
	ObjectManager::Update(dt);
	return this->GetObjects().size() == _uobj_num;
}

/****TimingActionGroup****/

TimingActionGroup::TimingActionGroup():
	_timeLimit(0.0f), _timer(0.0f)
{
	this->InitProperties();
}

TimingActionGroup::TimingActionGroup(float timeLimit):
	_timeLimit(timeLimit), _timer(0.0f)
{
	this->InitProperties();
}

TimingActionGroup::TimingActionGroup(const TimingActionGroup &other):
	ActionGroup(other),
	_timeLimit(other._timeLimit), _timer(other._timer)
{
	this->InitProperties();
}

void TimingActionGroup::InitProperties()
{
	this->RegisterProperty("TimeLimit", &_timeLimit);
	this->RegisterProperty("Timer", &_timer);
}

MM_IMPL_UPDATE(TimingActionGroup, dt)
{
	if (_timer >= _timeLimit) {
		_timer = 0.0f;
		return true;
	}
	
	_timer += dt;
	return ActionGroup::Update(dt);
}

/****ActionSleep****/

ActionSleep::ActionSleep():
	_delayTime(0), _cumTime(0)
{
	this->InitProperties();
}

ActionSleep::ActionSleep(float delayTime):
	_delayTime(delayTime), _cumTime(0)
{
	this->InitProperties();
}

ActionSleep::ActionSleep(float delayTime, const TimeoutEvent &onTimeout):
	_delayTime(delayTime), _cumTime(0), _onTimeout(onTimeout)
{
	this->InitProperties();
}

ActionSleep::ActionSleep(const ActionSleep &other):
	Action(other),
	_delayTime(other._delayTime), _cumTime(other._cumTime),
	_onTimeout(other._onTimeout)
{
	this->InitProperties();
}

void ActionSleep::InitProperties()
{
	this->RegisterProperty("DelayTime", &_delayTime);
	this->RegisterProperty("CumulativeTime", &_cumTime);
	this->RegisterProperty("OnTimeout", &_onTimeout,
						   PropertyPermission::Writeonly);
}

MM_IMPL_UPDATE(ActionSleep, dt)
{
	if (_cumTime < _delayTime) {
		_cumTime += dt;
		return false;
	}
	
	if (this->IsSetOnTimeout()) {
		_onTimeout(this);
		return _cumTime != 0;
	}
	
	return true;
}

void ActionSleep::Reset()
{
	_cumTime = 0;
}

void ActionSleep::OnUninstalled()
{
	this->Reset();
}

/****ActionCallback****/

ActionCallback::ActionCallback()
{
	this->InitProperties();
}

ActionCallback::ActionCallback(const CallbackEvent &event):
	_callback(event)
{
	this->InitProperties();
}

ActionCallback::ActionCallback(const ActionCallback &other):
	Action(other),
	_callback(other._callback)
{
	this->InitProperties();
}

void ActionCallback::InitProperties()
{
	this->RegisterProperty("Callback", &_callback,
						   PropertyPermission::Writeonly);
}

MM_IMPL_UPDATE(ActionCallback, dt)
{
	// 如果回调没有被设置，动作将直接被卸载
	if (this->IsSetCallback()) {
		_callback(this);
	}
	return true;
}

/****ActionRandom****/

ActionRandom::ActionRandom():
	_action(nullptr), _percentage(0)
{
	this->InitProperties();
}

ActionRandom::ActionRandom(Action *action, float per):
	_action(action), _percentage(per)
{
	MM_ASSERT(action);
	this->InitProperties();
}

ActionRandom::ActionRandom(const ActionRandom &other):
	Action(other),
	_action(other._action ? Object::Clone(other._action) : nullptr),
	_percentage(other._percentage)
{
	this->InitProperties();
}

void ActionRandom::InitProperties()
{
	this->RegisterProperty("TriggeredAction", &_action);
	this->RegisterProperty("Percentage", &_percentage);
}

ActionRandom::~ActionRandom()
{
	if (_action)
		_action->Destroy();
}

bool ActionRandom::OnInstalling()
{
	if (!Action::OnInstalling()) {
		return false;
	}
	if (!_action) {
		MM_THROW InstallingFailed("ActionRandom[OnInstalling]: 没有设置 Action");
		return false;
	}
	if (_percentage >= MathUtil::RandomInt(100)) {
		return true;
	}
	_action->SetRootTimeline(this->GetRootTimeline());
	return _action->Install();
}

void ActionRandom::OnUninstalled()
{
	_action->Uninstall();
}

MM_IMPL_UPDATE(ActionRandom, dt)
{
	return _action->Update(dt);
}

/**
 * 创建一个向 theMMEngine.ActionUpdater 注册 Action 的 ObjectManagerRegister
 */
Action *MakeActionUpdaterRegister(Action *ptr, bool isClone) {
	auto p = new ObjectManagerRegistrar<Action>(&(theMMEngine.ActionUpdater()), ptr);
	p->SetIsCloneObject(isClone);
	return p;
}

/**
 * 创建一个向 theMMEngine.BodyUpdater 注册 Action 的 ObjectManagerRegister
 */
Action *MakeBodyUpdaterRegister(Body *ptr, bool isClone) {
	auto p = new ObjectManagerRegistrar<Body>(&(theMMEngine.BodyUpdater()), ptr);
	p->SetIsCloneObject(isClone);
	return p;
}

/****MessageSender****/

MessageSender::MessageSender()
{
	this->InitProperties();
}

MessageSender::MessageSender(const String &msg):
	_msg(msg)
{
	this->InitProperties();
}

MessageSender::MessageSender(const MessageSender &other):
	Action(other),
	_msg(other._msg)
{
	this->InitProperties();
}

void MessageSender::InitProperties()
{
	this->RegisterProperty("Message", &_msg);
}

MM_IMPL_UPDATE(MessageSender, dt)
{
	theMMSwitchboard.Broadcast(new Message(_msg));
	return true;
}

/****BodyEmitter****/

BodyEmitter::BodyEmitter():
	_prototype(nullptr),
	_interval(0.0f), _cumInterval(0.0f),
	_emittedNumber(0), _cumNumber(0),
	_a_angle(0.0f), _a_speed(0.0f),
	_a_angleAddition(0.0f), _a_speedAddition(0.0f)
{
	this->InitProperties();
}

BodyEmitter::BodyEmitter(const BodyEmitter &other):
	Action(other),
	_prototype(other._prototype ? Object::Clone(other._prototype.get()) : nullptr),
	_onBodyCreated(other._onBodyCreated),
	_interval(other._interval),
	_cumInterval(other._cumInterval),
	_emittedNumber(other._emittedNumber),
	_cumNumber(other._cumNumber),
	_a_position(other._a_position),
	_a_angle(other._a_angle), _a_speed(other._a_speed),
	_a_angleAddition(other._a_angleAddition),
	_a_speedAddition(other._a_speedAddition)
{
	this->InitProperties();
}

void BodyEmitter::InitProperties()
{
	this->RegisterProperty("Prototype", &_prototype);
	this->RegisterProperty("OnBodyCreated", &_onBodyCreated,
						   PropertyPermission::Writeonly);
	this->RegisterProperty("Interval", &_interval);
	this->RegisterProperty("CumulativeInterval", &_cumInterval,
						   PropertyPermission::Readonly);
	this->RegisterProperty("EmittedNumber", &_emittedNumber);
	this->RegisterProperty("CumulativeNumber", &_cumNumber,
						   PropertyPermission::Readonly);
	this->RegisterProperty("AddiPosition", &_a_position);
	this->RegisterProperty("AddiAngle", &_a_angle);
	this->RegisterProperty("AddiSpeed", &_a_speed);
	this->RegisterProperty("AddiAngleAddition", &_a_angleAddition);
	this->RegisterProperty("AddiSpeedAddition", &_a_speedAddition);
}

void BodyEmitter::OnUninstalled()
{
	Action::OnUninstalled();
	
	if (_cumNumber > 1)
	{
		--_cumNumber;
		
		MM_PRO_BIN_OPER_PTR(_prototype, Position      , -, _a_position * _cumNumber);
		MM_PRO_BIN_OPER_PTR(_prototype, Angle         , -, _a_angle * _cumNumber);
		MM_PRO_BIN_OPER_PTR(_prototype, Speed         , -, _a_speed * _cumNumber);
		MM_PRO_BIN_OPER_PTR(_prototype, AngleAddition , -, _a_angleAddition * _cumNumber);
		MM_PRO_BIN_OPER_PTR(_prototype, SpeedAddition , -, _a_speedAddition * _cumNumber);
		
		_prototype->Calculate();
	}
	_cumNumber = 0;
	_cumInterval = 0;
}

Body *BodyEmitter::CreateBody()
{
	if (_cumNumber == _emittedNumber)
		return nullptr;
	
	++_cumNumber;
	
	auto newBullet = Object::Clone(_prototype.get());
	
	if (this->IsSetOnBodyCreated())
		_onBodyCreated(this, newBullet);
	
	if (_a_position != Vector2::Zero)
		MM_PRO_BIN_OPER_PTR(_prototype, Position, +, _a_position);
	
	if (_a_angleAddition != 0.0f)
		MM_PRO_BIN_OPER_PTR(_prototype, AngleAddition, +, _a_angleAddition);
	
	if (_a_speedAddition != 0.0f)
		MM_PRO_BIN_OPER_PTR(_prototype, SpeedAddition, +, _a_speedAddition);
	
	if (_a_angle != 0.0f)
		MM_PRO_BIN_OPER_PTR(_prototype, Angle, +, _a_angle);
	
	if (_a_speed != 0.0f)
		MM_PRO_BIN_OPER_PTR(_prototype, Speed, +, _a_speed);
	
	return newBullet;
}

bool BodyEmitter::Emit()
{
	Body *body = this->CreateBody();
	if (body == nullptr) {
		return true;
	}
	theMMBodyUpdater.Add(body);
	return false;
}

MM_IMPL_UPDATE(BodyEmitter, dt)
{
	MM_ASSERT(_prototype);
	
	if (_interval == 0.0f) {
		while (!Emit()) {}
		return true;
	}
	
	_cumInterval += dt;
	
	while (_cumInterval >= _interval) {
		_cumInterval -= _interval;
		if (this->Emit()) return true;
	}
	
	return false;
}

/****AnnularEmitter****/

AnnularEmitter::AnnularEmitter():
	_wayNumber(0), _radius(0.0f), _angleRange(360.0f)
{
	this->InitProperties();
}

AnnularEmitter::AnnularEmitter(const AnnularEmitter &other):
	BodyEmitter(other),
	_wayNumber(other._wayNumber), _radius(other._radius),
	_angleRange(other._angleRange)
{
	this->InitProperties();
}

void AnnularEmitter::InitProperties()
{
	this->RegisterProperty("WayNumber", &_wayNumber);
	this->RegisterProperty("Radius", &_radius);
	this->RegisterProperty("AngleRange", &_angleRange);
}

bool AnnularEmitter::Emit()
{
	Body *body = this->CreateBody();
	if (body == nullptr) {
		return true;
	}
	bool isSetRadius = (_radius != 0);
	bool isSetOnBodyCreated = this->IsSetOnBodyCreated();
	
	if (isSetRadius) {
		MM_PRO_BIN_OPER_PTR(body, Position, +,
						MathUtil::VectorFromAngle(body->GetAngle()) * _radius);
	}
	
	float angleAddi = _angleRange / _wayNumber;
	
	for (int i = 1; i != _wayNumber; ++i)
	{
		auto newbody = Object::Clone(body);
		newbody->SetAngle(body->GetAngle() + angleAddi * i);
		
		if (isSetRadius) {
			newbody->SetPosition (
				this->GetPrototype()->GetPosition() +
				MathUtil::VectorFromAngle(newbody->GetAngle()) * _radius
			);
		}
		
		if (isSetOnBodyCreated)
			_onBodyCreated(this, newbody);
		
		theMMEngine.BodyUpdater().Add(newbody);
	}
	// 在最后将 body 加入 BodyUpdater 是为了防止 BodyUpdater::Add 对
	// body->Timeline 执行 Install 从而干扰之后从 body 克隆的 Bodies
	theMMEngine.BodyUpdater().Add(body);
	return false;
}

/****IBodySelector****/

IBodySelector::IBodySelector():
	_layer(0), _isEnableLayer(true)
{
	this->InitProperties();
}

IBodySelector::IBodySelector(const IBodySelector &other):
	Action(other),
	_onSelected(other._onSelected), _layer(other._layer),
	_isEnableLayer(other._isEnableLayer)
{
	this->InitProperties();
}

void IBodySelector::InitProperties()
{
	this->RegisterProperty("OnSelected", &_onSelected);
	this->RegisterProperty("Layer", &_layer);
	this->RegisterProperty("IsEnableLayer", &_isEnableLayer);
}

IBodySelector::~IBodySelector()
{
}

/****CallbackSelector****/

CallbackSelector::CallbackSelector()
{
	this->InitProperties();
}

CallbackSelector::CallbackSelector(const IsSelectedEvent &event):
	_isSelected(event)
{
	this->InitProperties();
}

CallbackSelector::CallbackSelector(const CallbackSelector &other):
	IBodySelector(other),
	_isSelected(other._isSelected)
{
	this->InitProperties();
}

void CallbackSelector::InitProperties()
{
	this->RegisterProperty("IsSelected", &_isSelected,
						   PropertyPermission::Writeonly);
}

MM_IMPL_UPDATE(CallbackSelector, dt)
{
	if (!this->IsSetIsSelected() || !this->IsSetOnSelected())
		return true;
	
	for (auto each : theMMBodyUpdater.GetObjects())
	{
		auto obj = static_cast<Body*>(each);
		if ((!this->IsEnableLayer() || this->GetLayer() == obj->GetLayer()) && _isSelected(this, obj))
			this->_onSelected(this, obj);
	}
	return true;
}

/****CollisionSelector****/

CollisionSelector::CollisionSelector():
	_collider(nullptr)
{
	this->InitProperties();
}

CollisionSelector::CollisionSelector(const std::shared_ptr<ICollider> &collider):
	_collider(collider)
{
	this->InitProperties();
}

CollisionSelector::CollisionSelector(const CollisionSelector &other):
	CallbackSelector(other), _collider(Object::Clone(other._collider))
{
	this->InitProperties();
}

void CollisionSelector::InitProperties()
{
	this->RegisterProperty("Collider", &_collider);
}

bool CollisionSelector::OnInstalling()
{
	if (!CallbackSelector::OnInstalling()) {
		return false;
	}
	if (_collider == nullptr) {
		MM_THROW InstallingFailed("CollisionSelector[CollisionSelector]: 必须设置 Collider") ;
		return false;
	}
	return true;
}

MM_IMPL_UPDATE(CollisionSelector, dt)
{
	if (!this->IsSetOnSelected())
		return true;
	
	for (auto each : theMMBodyUpdater.GetObjects()) {
		auto body = static_cast<Body*>(each);
		if ((!this->IsEnableLayer() || this->GetLayer() == body->GetLayer()) && _collider->IsCollided(body)
			&& (!this->IsSetIsSelected() || this->_isSelected(this, body)) )
		{
			this->_onSelected(this, body);
		}
	}
	return true;
}

/****ReboundBoard****/

ReboundBoard::ReboundBoard():
	_layer(0), _isEnableLayer(false), _inc(0)
{
}

ReboundBoard::ReboundBoard(const Segment &seg):
	_layer(0), _isEnableLayer(false), _inc(0)
{
	this->SetSegment(seg);
}

ReboundBoard::ReboundBoard(const Segment &seg, const OnReboundingEvent &event):
	_layer(0), _isEnableLayer(false), _inc(0), _onRebounding(event)
{
	this->SetSegment(seg);
}

ReboundBoard::ReboundBoard(const ReboundBoard &other):
	Action(other),
	_layer(other._layer), _isEnableLayer(other._isEnableLayer),
	_inc(other._inc), _onRebounding(other._onRebounding),
	_seg(other._seg)
{
}

void ReboundBoard::InitProperties()
{
	this->RegisterProperty("OnRebounding", &_onRebounding,
						   PropertyPermission::Writeonly);
	this->RegisterProperty<Segment> (
		"Segment",
		std::bind(&ReboundBoard::SetSegment, this, std::placeholders::_1),
		std::bind(&ReboundBoard::GetSegment, this)
	);
	this->RegisterProperty("Layer", &_layer);
	this->RegisterProperty("IsEnableLayer", &_isEnableLayer);
}

void ReboundBoard::SetSegment(const Segment &v)
{
	_seg = v;
	
	float distance_x = v.B.X - v.A.X;
	float distance_y = v.B.Y - v.A.Y;
	
	if (distance_x > 0 && distance_y>=0)
		_inc = atan(distance_y/distance_x);
	
	else if (distance_x <= 0 && distance_y > 0)
		_inc = MathUtil::PiOver2 + atan(abs(distance_x) / distance_y);
	
	else if (distance_x < 0 && distance_y <= 0)
		_inc = MathUtil::Pi + atan(distance_y / distance_x);
	
	else if (distance_x >= 0 && distance_y < 0)
		_inc = 3 * MathUtil::PiOver2 + atan(distance_x / abs(distance_y));
	
	_inc = _inc / MathUtil::Pi * 180 - 90;
}

MM_IMPL_UPDATE(ReboundBoard, dt)
{
	for (auto each : theMMBodyUpdater.GetObjects()) {
		Bullet *bullet = nullptr;
		
		if (static_cast<Body*>(each)->IsBullet())
			bullet = static_cast<Bullet*>(each);
		else continue;
		
		if (this->IsEnableLayer() && bullet->GetLayer() != this->GetLayer())
			continue;
		
		if (bullet->IsCollided(_seg) &&
				(_onRebounding == nullptr || _onRebounding(this, bullet))) {
			if (bullet->GetCollidedReboundBoard() == this) {
				if (_seg.Intersects(bullet->GetPosition(), bullet->GetRadius())) {
					bullet->SetCollidedReboundBoard(nullptr);
				}
			}
			else {
				Body::Rebound(bullet, _inc);
				bullet->SetCollidedReboundBoard(this);
			}
		}
	}
	return false;
}

/****Animation****/

Animation::~Animation()
{
}

MM_END