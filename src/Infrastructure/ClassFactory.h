#pragma once

#include "../Util/StringUtil.h"

#define theMMClassFactory ::ModernMagic::ClassFactory::GetInstance()

MM_BEGIN

class ClassFactory
{
public:
	typedef std::function<Object*()>					ObjectCreator;
	typedef std::unordered_map<String, ObjectCreator>	CreatorContainer;

public:
	ClassFactory() {  }

	static ClassFactory &GetInstance()
	{
		static ClassFactory cf;
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
	void Register(const String &name, const ObjectCreator &creator) {
		_creators[name] = creator;
	}

	/**
	 * 通过类型工厂创建一个对象
	 */
	Object *CreateObject(const String &className) {
		auto iter = _creators.find(className);
		if (iter == _creators.end()) {
			return nullptr;
		}
		return iter->second();
	}

	/**
	 * 
	 */
	template <typename T>
	T *CreateObject(const String &className) {
		T *ptr = dynamic_cast<T*>(this->CreateObject(className));
		return ptr;
	}

private:
	CreatorContainer _creators;
};

MM_END