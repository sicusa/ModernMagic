#pragma once

#include "Util/StringUtil.h"
#include "Exceptions.h"

#include <unordered_map>

#define MM_TAG_ENABLE_TYPE(_className) \
	MM_PROPERTY_PBV_NS(IBodyType<_className>*, _type, Type); \
	public:\
	void SetTypePtr(IBodyType<_className>* ptr) { \
		if (_type != nullptr) _type->OnUnapplying(this); \
		_type = ptr; \
		if (_type != nullptr) _type->OnApplying(this); \
	} \
	virtual void SetType(const String &name) override { \
		auto p = TypeManager().GetType(name); \
		if (p == nullptr) throw UnregisteredBodyType(typeid(_className).name()); \
		this->SetTypePtr(p); return; \
	} \
	static BodyTypeManager<IBodyType<_className>> &TypeManager() { \
		static BodyTypeManager<IBodyType<_className>> obj; \
		return obj; \
	} \
	private:

#define MM_DEF_BODY_TEYP_BEGIN(_typeName, _appliedType, _typeString) \
	class _typeName : public IBodyType<_appliedType> \
	{ \
	public: \
		static IBodyType &GetInstance() { \
			static _typeName obj; \
			return obj; \
		} \
		virtual const char *GetName() override { return _typeString; } \

#define MM_DEF_BODY_TYPE_END \
	};

MM_BEGIN

template <typename T>
class IBodyType
{
public:
	typedef T AppliedBodyType;
	friend AppliedBodyType;
	
public:
	virtual ~IBodyType() {}
	
	/**
	 * 返回类型的名称
	 */
	virtual const char *GetName() = 0;

protected:
	/**
	 * 每一个 BodyType 只允许由 GetInstance 创建一个实例
	 */
	IBodyType() {}
	
	/**
	 * 当一个 Body SetType 至当前类型时，OnApplying 会被调用
	 */
	virtual void OnApplying(T *body) { }
	
	/**
	 * 当一个 Body 取消应用当前类型时，OnUnapplying 会被调用
	 */
	virtual void OnUnapplying(T *body) { }
};

template <typename T>
class BodyTypeManager
{
public:
	typedef std::unordered_map<String, T*> ContainerType;
	
	template <typename Type>
	void Register()
	{
		auto ins = &(Type::GetInstance());
		_registeredTypes[ins->GetName()] = ins;
	}
	
	T *GetType(const String &tname)
	{
		auto iter = _registeredTypes.find(tname);
		return iter != _registeredTypes.end() ? iter->second : nullptr;
	}
	
public:
	MM_PROPERTY_PBR(ContainerType, _registeredTypes, RegisteredTypes)
};

MM_END