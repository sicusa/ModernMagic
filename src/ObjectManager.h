#pragma once

#include "Timeline/Action.h"

MM_BEGIN

namespace Detail {
	/**
	 * ObjectManager 用于统一管理一组 Object
	 */
	class BasicObjectManager : public Action
	{
		MM_TAG_OBJECT(BasicObjectManager)
		MM_TAG_OVERRIDE_UPDATE
		
	public:
		typedef std::vector<Object*> ObjectContainer;
		
	public:
		BasicObjectManager();
		BasicObjectManager(const BasicObjectManager &other);
		~BasicObjectManager();
		
		/**
		 * 获取所有被注册的对象
		 *
		 * @return 存放被注册的对象的容器
		 */
		const ObjectContainer &GetObjects() const { return _objects; }
		
		/**
		 * 删除所有的对象
		 */
		void Clear();
		
	protected:
		/**
		 * 向 ObjectManager 添加一个对象，注册时调用 Listener::OnAdd 以确定
		 * 对象是否符合注册要求。
		 */
		Object *Add(Object *obj);
		
		virtual bool OnAdd          (Object*)   { return true; }
		virtual void OnRemove       (Object*)   { }
		virtual bool OnIterate      (Object*, float dt) { return false; }
		
	private:
		void InitProperties();
		ObjectContainer _objects;
	};
	
	namespace ObjectUpdater
	{
		bool OnAdd(Object *obj);
		bool OnIterate(Object *obj, float dt);
	}
}

template <typename T>
class ObjectManager : public Detail::BasicObjectManager
{
	MM_TAG_OBJECT(ObjectManager)
	
public:
	template <typename U>
	U *Add(U *obj)
	{ return static_cast<U*>(BasicObjectManager::Add(obj)); }
};

template <typename T>
class ObjectUpdater : public ObjectManager<UpdatableObject>
{
	MM_TAG_OBJECT(ObjectUpdater<T>)
	
protected:
	virtual bool OnAdd(Object *obj) override {
		return Detail::ObjectUpdater::OnAdd(obj);
	}
	virtual bool OnIterate(Object *obj, float dt) override {
		return Detail::ObjectUpdater::OnIterate(obj, dt);
	}
};

MM_END
	 