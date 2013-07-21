#include "stdafx.h"

#include "ObjectManager.h"
#include "Object.h"

MM_BEGIN

namespace Detail {
	
	BasicObjectManager::BasicObjectManager()
	{
		this->InitProperties();
	}
	
	BasicObjectManager::BasicObjectManager(const BasicObjectManager &other):
		Action(other)
	{
		this->InitProperties();

		for (auto &each : other._objects) {
			this->Add(Object::Clone(each));
		}
	}

	void BasicObjectManager::InitProperties()
	{
		this->RegisterProperty("Objects", &_objects,
							   PropertyPermission::Readonly);
	}
	
	BasicObjectManager::~BasicObjectManager()
	{
		for (auto each : _objects) {
			OnRemove(each);
			each->Destroy();
		}
	}
	
	Object *BasicObjectManager::Add(Object *obj)
	{
		MM_ASSERT(obj);
		
		if (OnAdd(obj)) {
			_objects.push_back(obj);
			return obj;
		}
		MM_ASSERT(false);
		return nullptr;
	}
	
	void BasicObjectManager::Clear()
	{
		for (auto each : _objects) {
			OnRemove(each);
			each->Destroy();
		}
		_objects.clear();
	}
	
	MM_IMPL_UPDATE(BasicObjectManager, dt)
	{
		for (int i = 0; i != _objects.size();)
		{
			// 如果 OnIterate 返回 true，当前对象就会被删除
			if (OnIterate(_objects[i], dt)) {
				_objects[i]->Destroy();
				_objects.erase(_objects.begin() + i);
			}
			else {
				++i;
			}
		}
		return false;
	}
	
	namespace ObjectUpdater {
		
		bool OnAdd(Object *obj)
		{
			MM_ASSERT(obj);
			// Object 能否被注册入 ObjectUpdater 成功取决于 Install 是否调用成功
			return static_cast<UpdatableObject*>(obj)->Install();
		}

		bool OnIterate(Object *obj, float dt)
		{
			MM_ASSERT(obj);
			auto ptr = static_cast<UpdatableObject*>(obj);

			if (!ptr->IsInstalled())
				return true;
			
			// 如果 Object 的更新函数返回 true，则卸载此对象
			if (ptr->Update(dt)) {
				ptr->Uninstall();
				return true;
			}
			
			return false;
		}
		
	} // namespace ObjectUpdaterDetail {
	
} // namespace Detail {


MM_END
