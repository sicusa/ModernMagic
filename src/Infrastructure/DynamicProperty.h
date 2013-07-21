#pragma once

#include "Exceptions.h"
#include <unordered_map>
#include <functional>

MM_BEGIN

enum class PropertyPermission
{
	Readonly,
	Writeonly,
	ReadAndWrite
};

struct PropertyInfo
{
	PropertyInfo():
		Hash(0),
		Permission(PropertyPermission::ReadAndWrite)
	{
	}

	PropertyInfo(const char *name, size_t hash,
				 PropertyPermission per = PropertyPermission::ReadAndWrite):
		Name(name), Hash(hash), Permission(per)
	{
	}

	String Name;
	size_t Hash;

	PropertyPermission Permission;
};

class DyBasicProperty
{
public:
	DyBasicProperty(const PropertyInfo *info);
	virtual ~DyBasicProperty() { }

	const String &GetName() const { return _info->Name; }
	size_t        GetHash() const { return _info->Hash; }
	PropertyPermission GetPermission() const { return _info->Permission; }

	template <typename T>
	const T &Get() const
	{
		if (_info->Permission == PropertyPermission::Writeonly) {
			throw DyPropertyReadException("Writeonly property");
		}
		if (_info->Hash != typeid(T).hash_code()) {
			throw DyPropertyCaseException("Type error");
		}
		return *static_cast<const T*>(this->GetData());
	}

	template <typename T>
	const T &UnsafeGet() const {
		return *static_cast<const T*>(this->GetData());
	}

	template <typename T>
	void Set(const T &value)
	{
		if (_info->Permission == PropertyPermission::Readonly) {
			throw DyPropertyWriteException("Readonly property");
		}
		if (_info->Hash != typeid(T).hash_code()) {
			throw DyPropertyCaseException("Type error");
		}
		this->SetData(static_cast<const void*>(&value));
	}

	template <typename T>
	void UnsafeSet(const T &value) {
		this->SetData(static_cast<const T*>(&value));
	}

protected:
	virtual const void *GetData() const = 0;
	virtual void SetData(const void *value) = 0;

private:
	const PropertyInfo *_info;
};

template <typename T>
class DyValueProperty : public DyBasicProperty
{
public:
	DyValueProperty(const PropertyInfo *info, T *dataptr);

protected:
	virtual const void *GetData() const override {
		return _ptr;
	}
	virtual void SetData(const void *value) override {
		MM_ASSERT(value);
		*_ptr = *static_cast<const T*>(value);
	}

private:
	T *_ptr;
};

template <typename T>
DyValueProperty<T>::DyValueProperty(const PropertyInfo *info, T *dataptr):
	DyBasicProperty(info), _ptr(dataptr)
{
	MM_ASSERT(dataptr);
}

template <typename T>
class DyFunctorProperty : public DyBasicProperty
{
public:
	typedef std::function<void(const T&)> Setter;
	typedef std::function<const T&()>     Getter;

	DyFunctorProperty(const PropertyInfo *info, const Setter &setter);
	DyFunctorProperty(const PropertyInfo *info, const Getter &getter);
	DyFunctorProperty(const PropertyInfo *info,
		const Setter &setter, const Getter &getter);

protected:
	virtual const void *GetData() const override {
		return &(_getter());
	}
	virtual void SetData(const void *value) override {
		_setter(*static_cast<const T*>(value));
	}

private:
	Setter _setter;
	Getter _getter;
};

template <typename T>
DyFunctorProperty<T>::
	DyFunctorProperty(const PropertyInfo *info, const Setter &setter):
	DyBasicProperty(info), _setter(setter)
{
}

template <typename T>
DyFunctorProperty<T>::
	DyFunctorProperty(const PropertyInfo *info, const Getter &getter):
	DyBasicProperty(info), _getter(getter)
{
}

template <typename T>
DyFunctorProperty<T>::
	DyFunctorProperty(const PropertyInfo *info,
	const Setter &setter, const Getter &getter):
DyBasicProperty(info), _setter(setter), _getter(getter)
{
}

class DyPropertyClass
{
public:
	typedef std::unordered_map<String, DyBasicProperty*> PropertyContainer;
	typedef std::unordered_map<String, std::auto_ptr<PropertyInfo> >    PropertyInfoContainer;

	DyPropertyClass();
	DyPropertyClass(const DyPropertyClass &other);
	~DyPropertyClass() { this->ClearProperties(); }

	void ClearProperties() {
		for (auto each : _pros) {
			delete each.second;
		}
		_pros.clear();
	}

	DyBasicProperty *GetProperty(const String &name) {
		return const_cast<DyBasicProperty*>(
			const_cast<const DyPropertyClass*>(this)->GetProperty(name)
		);
	}

	const DyBasicProperty *GetProperty(const String &name) const {
		auto iter = _pros.find(name);

		if (iter == _pros.end()) return nullptr;
		return iter->second;
	}

	template <typename T>
	void SetProperty(const String &name, T &&value) {
		this->GetProperty(name)->Set<T>(std::forward<T>(value));
	}

	const PropertyContainer &GetProperties() const {
		return _pros;
	}

protected:
	static PropertyInfo *GetPropertyInfo(const String &name);

	void RegisterProperty(DyBasicProperty *property) {
		MM_ASSERT(property);
		_pros[property->GetName()] = property;
	}

	template <typename T>
	void RegisterProperty(const String &name, T *ptr,
		PropertyPermission perm = PropertyPermission::ReadAndWrite)
	{
		struct PropertyInfo *info = GetPropertyInfo(name);

		if (!info->Hash) {
			info->Hash = typeid(T).hash_code();
			info->Permission = perm;
		}

		this->RegisterProperty(new DyValueProperty<T>(info, ptr));
	}

	template <typename T>
	void RegisterProperty(const String &name,
		const typename DyFunctorProperty<T>::Setter &setter,
		const typename DyFunctorProperty<T>::Getter &getter)
	{
		struct PropertyInfo *info = GetPropertyInfo(name);

		if (!info->Hash) {
			info->Hash = typeid(T).hash_code();
			info->Permission = PropertyPermission::ReadAndWrite;
		}
		this->RegisterProperty (
			new DyFunctorProperty<T>(info, setter, getter)
		);
	}

	template <typename T>
	void RegisterPropertySetter(const String &name,
		const typename DyFunctorProperty<T>::Setter &setter)
	{
		struct PropertyInfo *info = GetPropertyInfo(name);

		if (!info->Hash) {
			info->Hash = typeid(T).hash_code();
			info->Permission = PropertyPermission::Writeonly;
		}
		this->RegisterProperty (
			new DyFunctorProperty<T>(info, setter)
		);
	}

	template <typename T>
	void RegisterPropertyGetter(const String &name,
		const typename DyFunctorProperty<T>::Getter &getter)
	{
		struct PropertyInfo *info = GetPropertyInfo(name);

		if (!info->Hash) {
			info->Hash = typeid(T).hash_code();
			info->Permission = PropertyPermission::Readonly;
		}
		this->RegisterProperty (
			new DyFunctorProperty<T>(info, getter)
		);
	}

	bool RemoveProperty(const String &name)
	{
		auto iter = _pros.find(name);
		if (iter == _pros.end())
			return false;
		_pros.erase(iter);
		return true;
	}

private:
	PropertyContainer _pros;
	static PropertyInfoContainer _proinfos;
};

MM_END