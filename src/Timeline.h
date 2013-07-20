#pragma once

#include "ObjectManager.h"
#include "Action.h"

#include "Infrastructure/Interval.h"
#include "Infrastructure/Segment.h"
#include "Infrastructure/Color.h"

MM_BEGIN

/**
 * 时间轴（Timeline）表示 STG 游戏中一个子弹的一段动作，也可以表示整个游戏的关卡，它相当于一个任务列队，
 * 列队的基本单元是动作（Action），ModernMagic 提供了许多种不同的 Action 来实现各种游戏功能，
 * 引擎使用者也可以自行扩展或创建新的动作。
 */
class Timeline : public Action
{
	MM_TAG_OBJECT(Timeline)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	typedef std::vector<Action*> ContainerType;
	
public:
	Timeline();
	Timeline(const Timeline &other);
	~Timeline();
	
	/**
	 * 时间轴当前执行到的事件
	 */
	Action *CurrentAction() const { return _actions[_currentActionIndex]; }
	
	/**
	 * 切换当前执行的动作
	 *
	 * @param 指定的动作在容器中的索引
	 * @return 如果 index 指定的动作 Install 执行失败，返回 false，成功切换返回 true
	 */
	virtual bool SetCurrentAction(size_t index);
	
	/**
	 * 根据当前动作的索引的偏移来切换动作
	 */
	bool SetCurrentActionByOffset(int offset) {
		int index = _currentActionIndex + offset;
		return index >= 0 ? this->SetCurrentAction(index) : false;
	}
	
	/**
	 * 添加动作到时间轴中
	 */
	virtual void AddAction(Action *action);
	
	template <typename T>
	T *Add(T *action) {
		this->AddAction(action);
		return action;
	}
	
	/**
	 * 移除一个动作
	 */
	virtual void Remove(size_t index);
	
	// Remove(Action*) 会找到 ptr 在容器中的 index 然后调用 Remove(size_t)
	void Remove(Action *ptr);
	
	/**
	 * 清空时间轴
	 */
	virtual void Clear();
	
	/**
	 * 切换到下一个能够成功执行安装事件（Install）的动作
	 *
	 * @return 如果接下来的动作都安装失败或者已经执行完了所有的动作，返回 false，成功切换返回 true
	 */
	virtual bool NextAction();
	
	virtual void SetRootTimeline(Timeline *tl) override;
	
protected:
	virtual void OnUninstalled() override;
	
public:
	// 存放动作的容器
	MM_PROPERTY_PBR_NS(ContainerType, _actions, Actions)
	
	// 时间轴当前执行到的事件的索引
	MM_PROPERTY_PBV_NS(size_t, _currentActionIndex, CurrentActionIndex)
	
	// 绑定当前时间轴的 object
	MM_PROPERTY_PBV(Object*, _bindingObject, BindingObject)
	
private:
	void InitProperties();
};

/**
 * 允许重复执行一组动作的时间轴
 */
class LoopTimeline : public Timeline
{
	MM_TAG_OBJECT(LoopTimeline)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	LoopTimeline();
	LoopTimeline(size_t loopNumber);
	LoopTimeline(const LoopTimeline &other);
	
public:
	// 循环的次数
	MM_PROPERTY_PBV(size_t, _loopNumber, LoopNumber)
	
	// 累计的循环次数
	MM_PROPERTY_PBV(size_t, _cumNumber, CumulativeNumber)
	
private:
	void InitProperties();
};

/**
 * 同时执行多个动作的动作组
 */
class ActionGroup : public ObjectManager<Action>
{
	MM_TAG_OBJECT(ActionGroup)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	ActionGroup();
	
	virtual void SetRootTimeline(Timeline *tl) override;
	
protected:
	virtual void OnCloneObject  (Object*) override;
	virtual bool OnAdd          (Object*) override;
	virtual void OnRemove       (Object*) override;
	virtual bool OnIterate      (Object*,float) override;
	virtual bool OnInstalling   () override;
	virtual void OnUninstalled  () override;
	
private:
	size_t _uobj_num;
};

/**
 * 定时动作组
 *
 * 给定一个时间上限，超过该上限则无论动作组内的动作全部执行与否都强制卸载动作组
 */
class TimingActionGroup : public ActionGroup
{
	MM_TAG_OBJECT(TimingActionGroup)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	TimingActionGroup();
	TimingActionGroup(float timeLimit);
	TimingActionGroup(const TimingActionGroup &other);
	
	MM_PROPERTY_PBV(float, _timeLimit, TimeLimit)
	MM_PROPERTY_PBV(float, _timer, Timer)
	
private:
	void InitProperties();
};

/**
 * 在相连的动作间提供暂停的动作
 */
class ActionSleep : public Action
{
	MM_TAG_OBJECT(ActionSleep)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	typedef std::function<void(ActionSleep*)> TimeoutEvent;
	
public:
	ActionSleep();
	ActionSleep(float delayTime);
	ActionSleep(float delayTime, const TimeoutEvent &onTimeout);
	ActionSleep(const ActionSleep &other);
	
	/**
	 * 重新开始计时
	 */
	virtual void Reset();
	
protected:
	virtual void OnUninstalled() override;
	
public:
	// 在此动作上延迟的时间
	MM_PROPERTY_PBV(float, _delayTime, DelayTime)
	
	// 累积的时间
	MM_PROPERTY_PBV(float, _cumTime, CumulativeTime)
	
	// 当累积的时间超过设定的延迟时间时调用此事件
	MM_EVENT(TimeoutEvent, _onTimeout, OnTimeout)
	
private:
	void InitProperties();
};

/**
 * 在动作队列中提供回调操作
 */
class ActionCallback : public Action
{
	MM_TAG_OBJECT(ActionCallback)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	typedef std::function<void(ActionCallback*)> CallbackEvent;
	
public:
	ActionCallback();
	ActionCallback(const CallbackEvent &event);
	ActionCallback(const ActionCallback &other);
	
public:
	/**
	 * 在 ActionCallback 的 Update 中会调用的回调事件，其返回值就是 Update 函数的返回值
	 * 如果事件返回 true，此事件就会被卸载，返回 false 则在下一次调用 Update 时继续调用回调事件
	 */
	MM_EVENT(CallbackEvent, _callback, Callback)
	
private:
	void InitProperties();
};

/**
 * 随机发生动作
 */
class ActionRandom : public Action
{
	MM_TAG_OBJECT(ActionRandom)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	ActionRandom();
	ActionRandom(Action *action, float per);
	ActionRandom(const ActionRandom &other);
	~ActionRandom();
	
public:
	MM_PROPERTY_PBV(Action*, _action, TriggeredAction);
	
	// 动作被执行的百分率
	MM_PROPERTY_PBV(int, _percentage, Percentage);
	
protected:
	virtual bool OnInstalling()  override;
	virtual void OnUninstalled() override;
	
private:
	void InitProperties();
};

/**
 * 向一个指定的 ObjectManager 注册一个 Object 的动作
 */
template <typename ObjectType>
class ObjectManagerRegistrar : public Action
{
	MM_TAG_OBJECT(ObjectManagerRegistrar)
	
public:
	ObjectManagerRegistrar();
	ObjectManagerRegistrar(ObjectUpdater<ObjectType> *mgr, ObjectType *ptr);
	ObjectManagerRegistrar(const ObjectManagerRegistrar &other);
	~ObjectManagerRegistrar();
	
	virtual bool Update(float dt) override
	{
		MM_ASSERT(_objMgr);
		MM_ASSERT(_obj);
		
		_objMgr->Add(_isCloneObj ? Object::Clone(_obj) : _obj);
		
		// 如果 Object 是被直接添加进 ObjectManager 的，此动作会在添加后把 Object 设为 nullptr
		// 以防止将同一个 Object 重复添加进 ObjectManager
		if (!_isCloneObj)
			_obj = nullptr;
		
		return true;
	}
	
public:
	MM_PROPERTY_PBV(ObjectUpdater<ObjectType>*, _objMgr, ObjectManager)
	MM_PROPERTY_PBV(ObjectType*, _obj, Object)
	
	// 在注册时是否创建 Object 的副本，默认为 true
	MM_PROPERTY_BOOL(_isCloneObj, IsCloneObject)
	
private:
	void InitProperties();
};

template <typename ObjectType>
ObjectManagerRegistrar<ObjectType>::
ObjectManagerRegistrar():
	_objMgr(nullptr), _obj(nullptr), _isCloneObj(true)
{
	this->InitProperties();
}

template <typename ObjectType>
ObjectManagerRegistrar<ObjectType>::
ObjectManagerRegistrar(ObjectUpdater<ObjectType> *mgr, ObjectType *ptr):
	_objMgr(mgr), _obj(ptr), _isCloneObj(true)
{
	this->InitProperties();
}

template <typename ObjectType>
ObjectManagerRegistrar<ObjectType>::
ObjectManagerRegistrar(const ObjectManagerRegistrar &other):
	Action(other),
	_objMgr(other._objMgr),
	_obj(_obj != nullptr && _isCloneObj ? Object::Clone(_obj) : _obj),
	_isCloneObj(other._isCloneObj)
{
	this->InitProperties();
}

template <typename ObjectType>
void ObjectManagerRegistrar<ObjectType>::InitProperties()
{
	this->RegisterProperty("ObjectManager", &_objMgr);
	this->RegisterProperty("IsClone", &_isCloneObj);
	this->RegisterProperty("Object", &_obj);
}

template <typename ObjectType>
ObjectManagerRegistrar<ObjectType>::~ObjectManagerRegistrar() {
	if (_obj != nullptr && _isCloneObj)
		_obj->Destroy();
}

/**
 * 创建一个向 theMMEngine.ActionUpdater 注册 Action 的 ObjectManagerRegister
 */
Action *MakeActionUpdaterRegister(Action *ptr, bool isClone = true);

/**
 * 创建一个向 theMMEngine.BodyUpdater 注册 Action 的 ObjectManagerRegister
 */
Action *MakeBodyUpdaterRegister(Body *ptr, bool isClone = true);

/**
 * 向 theMMSwitchboard 发送消息
 */
class MessageSender : public Action
{
	MM_TAG_OBJECT(MessageSender)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	MessageSender();
	MessageSender(const String &msg);
	MessageSender(const MessageSender &other);
	
	MM_PROPERTY_PBR(String, _msg, Message);
	
private:
	void InitProperties();
};

/**
 * Body 发射器
 */
class BodyEmitter : public Action
{
	MM_TAG_OBJECT(BodyEmitter)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	typedef std::function<void(BodyEmitter*,Body*)> OnBodyCreatedEvent;
	
public:
	BodyEmitter();
	BodyEmitter(const BodyEmitter &other);
	
public:
	// 发射的Body原型
	MM_PROPERTY_PBR(std::shared_ptr<Body>, _prototype, Prototype)
	
	// Body 创建完成后调用的事件
	MM_EVENT(OnBodyCreatedEvent, _onBodyCreated, OnBodyCreated)
	
	// 发射的间隔
	MM_PROPERTY_PBV(float, _interval, Interval)
	
	// 当前累积的时间
	MM_PROPERTY_PBV_NS(float, _cumInterval, CumulativeInterval)
	
	// 规定发射的次数
	MM_PROPERTY_PBV(int, _emittedNumber, EmittedNumber)
	
	// 发射器累计发射的次数
	MM_PROPERTY_PBV_NS(int, _cumNumber, CumulativeNumber)
	
	// 坐标加量
	MM_PROPERTY_PBR(Vector2, _a_position, AddiPosition)
	
	// 角度加量
	MM_PROPERTY_PBV(float, _a_angle, AddiAngle)
	
	// 速度加量
	MM_PROPERTY_PBV(float, _a_speed, AddiSpeed)
	
	// 角度加量的加量
	MM_PROPERTY_PBV(float, _a_angleAddition, AddiAngleAddition)
	
	// 速度加量的加量
	MM_PROPERTY_PBV(float, _a_speedAddition, AddiSpeedAddition)
	
protected:
	virtual void OnUninstalled() override;
	
	// 创建将要发射的弹幕
	Body *CreateBody();
	
	/**
	 * 发射弹幕
	 *
	 * @return 如果返回 true 则发射器停止发射
	 */
	virtual bool Emit();
	
private:
	void InitProperties();
};

/**
 * 环状发射器
 */
class AnnularEmitter : public BodyEmitter
{
	MM_TAG_OBJECT(AnnularEmitter)
	
public:
	AnnularEmitter();
	AnnularEmitter(const AnnularEmitter &other);
	
	// 子弹的条数
	MM_PROPERTY_PBV(int, _wayNumber, WayNumber)
	
	// 发射器半径
	MM_PROPERTY_PBV(float, _radius, Radius)
	
	// 发射范围
	MM_PROPERTY_PBV(float, _angleRange, AngleRange)
	
protected:
	virtual bool Emit() override;
	
private:
	void InitProperties();
};

/**
 * 选中器
 */
class IBodySelector : public Action
{
	MM_TAG_ABSTRACT(IBodySelector)
	
public:
	IBodySelector();
	IBodySelector(const IBodySelector &other);
	
	typedef std::function<void(IBodySelector*, Body*)> OnSelectedEvent;
	
public:
	// 当一个 Body 被选中时调用的事件
	MM_EVENT(OnSelectedEvent, _onSelected, OnSelected)
	
	// 选中器所在的层
	MM_PROPERTY_PBV(int, _layer, Layer)
	
	// 是否启用层挑选
	MM_PROPERTY_BOOL(_isEnableLayer, IsEnableLayer)
	
private:
	void InitProperties();
};

/**
 * 回调函数选中器
 */
class CallbackSelector : public IBodySelector
{
	MM_TAG_OBJECT(CallbackSelector)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	typedef std::function<bool(CallbackSelector*, Body*)> IsSelectedEvent;
	
	CallbackSelector();
	CallbackSelector(const IsSelectedEvent &event);
	CallbackSelector(const CallbackSelector &other);
	
public:
	MM_EVENT(IsSelectedEvent, _isSelected, IsSelected)
	
private:
	void InitProperties();
};

/**
 * 碰撞选中器
 */
class ICollider;
class CollisionSelector : public CallbackSelector
{
	MM_TAG_OBJECT(CollisionSelector)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	CollisionSelector();
	CollisionSelector(ICollider *collider);
	CollisionSelector(const CollisionSelector &other);

	MM_PROPERTY_PBR(std::shared_ptr<ICollider>, _collider, Collider)
	
private:
	void InitProperties();
};

/**
 * 反弹板
 *
 * ReboundBoard 的 Update 函数一直返回 false
 * 如果没有手动操作，反弹板将会一直不被卸载
 * 当前只支持子弹的反弹
 */
class ReboundBoard : public Action
{
	MM_TAG_OBJECT(ReboundBoard)
	MM_TAG_OVERRIDE_UPDATE
	
public:
	typedef std::function<bool(ReboundBoard*,Body*)> OnReboundingEvent;
	
public:
	ReboundBoard();
	ReboundBoard(const Segment &seg);
	ReboundBoard(const Segment &seg, const OnReboundingEvent &event);
	ReboundBoard(const ReboundBoard &other);
	
public:
	// 当反弹板与 Body 发生碰撞时调用的事件，返回 false 则忽略此次反弹
	MM_EVENT(OnReboundingEvent, _onRebounding, OnRebounding)
	
	// 使用的线段
	MM_PROPERTY_PBR_US(Segment, _seg, Segment)
	
	// 选中器所在的层
	MM_PROPERTY_PBV(int, _layer, Layer)
	
	// 是否启用层挑选
	MM_PROPERTY_BOOL(_isEnableLayer, IsEnableLayer)
	
private:
	void InitProperties();
	float _inc;
};

/**
 * 属性设置器
 */
template <typename T>
class __ChangeProperty : public Action
{
public:
	__ChangeProperty();
	__ChangeProperty(const String &proname, const T &v);
	__ChangeProperty(const __ChangeProperty &other);
	
public:
	MM_PROPERTY_PBR(String, _proname, PropertyName)
	MM_PROPERTY_PBV_NS(DyBasicProperty*, _property, ChangedProperty)
	MM_PROPERTY_PBR(T, _value, Value)
	
private:
	virtual bool OnInstalling() override
	{
		auto rootobj = this->GetRootObject();
		if (!rootobj)
			return false;

		_property = rootobj->GetProperty(_proname);

		if (!_property || typeid(T).hash_code() != _property->GetHash() ||
			_property->GetPermission() == PropertyPermission::Readonly) {
			return false;
		}
		return true;
	}
	void InitProperties();
};

template <typename T>
__ChangeProperty<T>::__ChangeProperty():
	_property(nullptr)
{
	this->InitProperties();
}

template <typename T>
__ChangeProperty<T>::__ChangeProperty(const String &proname, const T &value):
	_proname(proname), _property(nullptr), _value(value)
{
	this->InitProperties();
}

template <typename T>
__ChangeProperty<T>::__ChangeProperty(const __ChangeProperty &other):
	Action(other),
	_proname(other._proname), _property(other._property), _value(other._value)
{
	this->InitProperties();
}

template <typename T>
void __ChangeProperty<T>::InitProperties()
{
	this->RegisterProperty("Property", &_proname);
	this->RegisterProperty("Value", &_value);
}

template <typename T>
class ChangeTo : public __ChangeProperty<T>
{
	MM_TAG_OBJECT(ChangeTo)

public:
	ChangeTo() {}
	ChangeTo(const String &proname, const T &v):
		__ChangeProperty<T>::__ChangeProperty(proname, v)
	{
	}

	virtual bool Update(float dt) override
	{
		this->GetChangedProperty()->UnsafeSet (
			this->GetValue()
		);
		return true;
	}
};

template <typename T>
class ChangeBy : public __ChangeProperty<T>
{
	MM_TAG_OBJECT(ChangeBy)

public:
	ChangeBy() {}
	ChangeBy(const String &proname, const T &v):
		__ChangeProperty<T>::__ChangeProperty(proname, v)
	{
	}

	virtual bool Update(float dt) override
	{
		this->GetChangedProperty()->UnsafeSet (
			this->GetChangedProperty()->UnsafeGet<T>() +
			this->GetValue()
		);
		return true;
	}
};

/**
 * 动画组件
 */
class Animation : public Action
{
	MM_TAG_ABSTRACT(Animation)
};

/**
 * 回调动画
 */
template <typename T>
class CallbackAnimation : public Animation
{
	MM_TAG_OBJECT(CallbackAnimation<T>)
	
public:
	typedef std::function<void(CallbackAnimation*,T)> OnUpdateEvent;
	
public:
	CallbackAnimation();
	CallbackAnimation(const T &from, const T &to, float dur,
					  IntervalFunc func = IF_Linear);
	CallbackAnimation(const Interval<T> &interval);
	CallbackAnimation(const CallbackAnimation &other);
	
	virtual bool Update(float dt) override {
		_onUpdate(this, _interval.Step(dt));
		return _interval.ShouldStep();
	}
	
private:
	MM_PROPERTY_PBR(Interval<T>, _interval, Interval)
	MM_EVENT(OnUpdateEvent, _onUpdate, OnUpdate)
	
protected:
	virtual bool OnInstalling() override {
		return Animation::OnInstalling() && _onUpdate != nullptr;
	}
	
private:
	void InitProperties();
};

template <typename T>
CallbackAnimation<T>::CallbackAnimation()
{
	this->InitProperties();
}

template <typename T>
CallbackAnimation<T>::CallbackAnimation(const T &from, const T &to, float dur,
										IntervalFunc func):
	_interval(from, to, dur, func)
{
	this->InitProperties();
}

template <typename T>
CallbackAnimation<T>::CallbackAnimation(const Interval<T> &interval):
	_interval(interval)
{
	this->InitProperties();
}

template <typename T>
CallbackAnimation<T>::CallbackAnimation(const CallbackAnimation &other):
	_interval(other._interval), _onUpdate(other._onUpdate)
{
	this->InitProperties();
}

template <typename T>
void CallbackAnimation<T>::InitProperties()
{
	this->RegisterProperty("Interval", &_interval);
	this->RegisterProperty("OnUpdate", &_onUpdate,
						   PropertyPermission::Writeonly);
}

/**
 * 属性动画
 */
template <typename T>
class __PropertyAnimation : public Animation
{
	MM_TAG_ABSTRACT(__PropertyAnimation)
	
public:
	typedef Interval<T> IntervalType;

public:
	__PropertyAnimation();
	__PropertyAnimation(const String &proname, const T &end, float dur,
						IntervalFunc func);
	__PropertyAnimation(const String &proname, const T &start, const T &end,
						float dur, IntervalFunc func);
	__PropertyAnimation(const String &proname, const Interval<T> &interval):
		_interval(interval), _proname(proname), _startSetted(true), _property(nullptr)
	{
		this->InitProperties();
	}
	__PropertyAnimation(const __PropertyAnimation &other);
	
private:
	MM_PROPERTY_PBR(IntervalType, _interval, Interval)
	MM_PROPERTY_PBR(String, _proname, PropertyName)
	MM_PROPERTY_PBV(DyBasicProperty*, _property, ChangedProperty)
	
protected:
	virtual bool OnInstalling() override
	{
		if (!Animation::OnInstalling())
			return false;
		
		auto rootobj = this->GetRootObject();
		if (!rootobj)
			return false;

		_property = rootobj->GetProperty(_proname);
		
		if (!_property || typeid(T).hash_code() != _property->GetHash() ||
			_property->GetPermission() == PropertyPermission::Readonly) {
			return false;
		}
		if (!_isSetStart) {
			if (_property->GetPermission() != PropertyPermission::ReadAndWrite)
				return false;

			_interval.SetStart(_property->UnsafeGet<T>());
		}
		return true;
	}
	virtual void OnUninstalled() override
	{
		Animation::OnUninstalled();
		_interval.Restart();
	}
	IntervalType &Interval() { return _interval; }
	
protected:
	bool _isSetStart;
	
private:
	void InitProperties();
};

template <typename T>
void __PropertyAnimation<T>::InitProperties()
{
	this->RegisterProperty<T>("From",
		std::bind(&IntervalType::SetStart, &_interval, std::placeholders::_1),
		std::bind(&IntervalType::GetStart, &_interval)
	);
	this->RegisterProperty<T>("To",
		std::bind(&IntervalType::SetEnd, &_interval, std::placeholders::_1),
		std::bind(&IntervalType::GetEnd, &_interval)
	);
	this->RegisterProperty<float>("Duration",
		std::bind(&IntervalType::SetDuration, &_interval, std::placeholders::_1),
		std::bind(&IntervalType::GetDuration, &_interval)
	);
	this->RegisterProperty<IntervalFunc>("Effect",
		std::bind(&IntervalType::SetIntervalFunc, &_interval, std::placeholders::_1),
		std::bind(&IntervalType::GetIntervalFunc, &_interval)
	);
	this->RegisterProperty("Property", &_proname);
}

template <typename T>
__PropertyAnimation<T>::__PropertyAnimation():
	_isSetStart(true), _property(nullptr)
{
	this->InitProperties();
}

template <typename T>
__PropertyAnimation<T>::
__PropertyAnimation(const String &proname, const T &start, const T &end, float dur,
					IntervalFunc func):
	_interval(start, end, time, func), _proname(proname),
	_isSetStart(true), _property(nullptr)
{
	this->InitProperties();
}

template <typename T>
__PropertyAnimation<T>::
__PropertyAnimation(const String &proname, const T &end, float dur,
					IntervalFunc func):
	_interval(T(), end, dur, func), _proname(proname),
	_isSetStart(false), _property(nullptr)
{
	this->InitProperties();
}

template <typename T>
__PropertyAnimation<T>::
__PropertyAnimation(const __PropertyAnimation &other):
	Animation(other),
	_interval(other._interval), _proname(other._proname),
	_property(other._property), _isSetStart(other._isSetStart)
{
	this->InitProperties();
}

template <typename T>
__PropertyAnimation<T>::~__PropertyAnimation()
{
}

template <typename T>
class AnimateTo : public __PropertyAnimation<T>
{
	MM_TAG_OBJECT(AnimateTo)
	
public:
	AnimateTo() {}
						   
	AnimateTo(const String &proname, const T &start, const T &end, float dur,
						IntervalFunc func = IF_Linear):
		__PropertyAnimation<T>(proname, start, end, dur, func)
	{
	}
	AnimateTo(const String &proname, const T &end, float dur,
			  IntervalFunc func = IF_Linear):
		__PropertyAnimation<T>(proname, end, dur, func)
	{
	}
	
	virtual bool Update(float dt) override
	{
		this->GetChangedProperty()->Set(this->Interval().Step(dt));
		return !this->GetInterval().ShouldStep();
	}
};

template <typename T>
class AnimateBy : public __PropertyAnimation<T>
{
	MM_TAG_OBJECT(AnimateBy)
	
public:
	AnimateBy() {}
	
	AnimateBy(const String &proname,
						const T &start, const T &end, float dur,
						IntervalFunc func = IF_Linear):
		__PropertyAnimation<T>(proname, start, end, dur, func)
	{
	}
	AnimateBy(const String &proname, const T &end, float dur,
						IntervalFunc type = IF_Linear):
		__PropertyAnimation<T>(proname, end, dur, type)
	{
		this->_isSetStart = true;
	}
	
	virtual bool Update(float dt) override
	{
		this->GetChangedProperty()->UnsafeSet (
			_old + this->Interval().Step(dt)
		);
		return !this->GetInterval().ShouldStep();
	}
	
protected:
	virtual bool OnInstalling() override
	{
		if (!__PropertyAnimation<T>::OnInstalling())
			return false;
		
		_old = this->GetChangedProperty()->template UnsafeGet<T>();
		return true;
	}
	
private:
	T _old;
};

MM_END
