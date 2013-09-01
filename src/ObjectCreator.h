#pragma once

#include "Util/StringUtil.h"

#define theMMObjectCreator ::ModernMagic::ObjectCreator::GetInstance()

MM_BEGIN

class ObjectCreator
{
public:
	typedef std::function<Object*()>				CreatorFunc;
	typedef std::unordered_map<String, CreatorFunc>	CreatorContainer;

public:
	static ObjectCreator &GetInstance()
	{
		static ObjectCreator cf;
		return cf;
	}

	/**
	 * 向 ClassFactory 注册一个类，该类必须拥有有效的默认构造函数
	 */
	template <typename T>
	void Register(const String &name) {
		this->Register(name, []() -> Object* {
			return new T();
		});
	}

	/**
	 * 向 ClassFactory 注册一个类
	 *
	 * @param 类型的字符串名
	 * @param 能用创建此类型的对象的函数
	 */
	void Register(const String &name, const CreatorFunc &creator);

	/**
	 * 通过类型工厂创建一个对象
	 */
	Object *Create(const String &className);

	template <typename T>
	T *Create(const String &className) {
		T *ptr = dynamic_cast<T*>(this->CreateObject(className));
		return ptr;
	}

	template <typename T>
	T *UnsafeCreate(const String &className) {
		T *ptr = static_cast<T*>(this->CreateObject(className));
		return ptr;
	}

private:
	ObjectCreator();
	CreatorContainer _creators;
};

MM_END