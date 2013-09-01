#include "stdafx.h"

#include "Timeline.h"
#include "Engine.h"

MM_BEGIN

/****Timeline****/

Timeline::Timeline():
	_currentActionIndex(0),
	_bindingObject(nullptr)
{
	Action::SetRootTimeline(this);
	this->InitProperties();
}

Timeline::Timeline(const Timeline &other):
	Action(other),
	_currentActionIndex(other._currentActionIndex),
	_bindingObject(nullptr)
{
	Timeline *rtl = other.GetRootTimeline();
	Action::SetRootTimeline(rtl != &other ? rtl : this);
	Object::Clone(other._actions, &_actions, this->GetRootTimeline(), this);
	
	this->InitProperties();
}

void Timeline::InitProperties()
{
	this->RegisterProperty("Actions", &_actions,
						   PropertyPermission::Readonly);
	this->RegisterPropertySetter<String>("BindTo", [this](const String &value) {
		auto iter = theMMNamedObjects.find(value);
		if (iter == theMMNamedObjects.end()) {
			return;
		}
		this->_bindingObject = iter->second;
	});
}

Timeline::~Timeline()
{
	for (auto each : _actions) each->Destroy();
}

void Timeline::SetRootTimeline(Timeline *tl)
{
	MM_ASSERT(tl);
	
	Action::SetRootTimeline(tl);
	
	for (auto each : _actions) {
		each->SetRootTimeline(tl);
	}
}

bool Timeline::SetCurrentAction(size_t index)
{
	MM_ASSERT(index >= _actions.size());
	
	MM_TRY {
		if (!_actions[index]->Install()) {
			return false;
		}
	}
	MM_CATCH (InstallingFailed &e) {
		MM_THROW InstallingFailed("Timeline[SetCurrentAction]: 指定的 Action 安装失败 -->\n    " + e.str());
	}
	
	this->CurrentAction()->Uninstall();
	_currentActionIndex = index;
	return true;
}

void Timeline::AddAction(Action *action)
{
	MM_ASSERT(action);
	
	action->SetRootTimeline(this->GetRootTimeline());
	action->SetParent(this);

	_actions.push_back(action);
}

void Timeline::Remove(size_t index)
{
	MM_ASSERT(index >= _actions.size());
	
	auto iter = _actions.begin() + index;
	
	if ((*iter)->IsInstalled())
		(*iter)->Uninstall();
	
	_actions.erase(iter);
}

void Timeline::Remove(Action *ptr)
{
	MM_ASSERT(ptr);
	
	auto iter = std::find(_actions.begin(), _actions.end(), ptr);
	
	if (iter == _actions.end()) {
		return;
	}
	this->Remove(static_cast<size_t>(_actions.begin() - iter));
}

void Timeline::Clear()
{
	if (this->_actions.empty()) {
		return;
	}
	
	if (this->IsInstalled())
		this->CurrentAction()->Uninstall();
	
	for (auto each : _actions) {
		each->Destroy();
	}
	
	this->_actions.clear();
}

bool Timeline::NextAction()
{
	this->CurrentAction()->Uninstall();
	
	size_t size = _actions.size();
	while (++_currentActionIndex < size) {
		MM_TRY {
			if (CurrentAction()->Install()) {
				return true;
			}
		}
		MM_CATCH (Exception &e) {
			MM_THROW(InstallingFailed("Timeline[NextAction]: 子动作安装失败 -->\n    " + e.str()));
		}
	}
	return false;
}

void Timeline::OnUninstalled()
{
	Action::OnUninstalled();
	_currentActionIndex = 0;
}

MM_IMPL_UPDATE(Timeline, dt)
{
	if (_currentActionIndex == _actions.size())
		return true;

	if (_currentActionIndex == 0) {
		CurrentAction()->Install();
	}
	
	while (!CurrentAction()->IsInstalled() || CurrentAction()->Update(dt)) {
		if (!NextAction()) {
			return true;
		}
	}
	
	return false;
}

/****LoopTimeline****/

LoopTimeline::LoopTimeline():
	_loopNumber(0), _cumNumber(0)
{
	this->InitProperties();
}

LoopTimeline::LoopTimeline(int loopNumber):
	_loopNumber(loopNumber), _cumNumber(0)
{
	this->InitProperties();
}

LoopTimeline::LoopTimeline(const LoopTimeline &other):
	Timeline(other),
	_loopNumber(other._loopNumber), _cumNumber(other._cumNumber)
{
	this->InitProperties();
}

MM_IMPL_UPDATE(LoopTimeline, dt)
{
	if (Timeline::Update(dt)) {
		Timeline::Uninstall();
		
		if (++_cumNumber == _loopNumber) {
			return true;
		}
		if (!Timeline::Install()) {
			return false;
		}
	}
	return false;
}

void LoopTimeline::InitProperties()
{
	this->RegisterProperty("LoopNumber", &_loopNumber);
	this->RegisterProperty("CumulativeNumber", &_cumNumber);
}

MM_END
