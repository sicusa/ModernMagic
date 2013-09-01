#pragma once

#include "Action.h"

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
	LoopTimeline(int loopNumber);
	LoopTimeline(const LoopTimeline &other);
	
public:
	// 循环的次数
	MM_PROPERTY_PBV(int, _loopNumber, LoopNumber)
	
	// 累计的循环次数
	MM_PROPERTY_PBV(int, _cumNumber, CumulativeNumber)
	
private:
	void InitProperties();
};

MM_END
