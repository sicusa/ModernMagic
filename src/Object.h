#pragma once

#include "Exceptions.h"
#include "DynamicProperty.h"

/**
 * MM_TAG_CLONEABLE 用以支持 Object 派生类的 Clone 函数实现。
 * 所有 Object 的派生类都必须提供可使用的拷贝构造函数。
 */
#define MM_TAG_CLONEABLE(_className)\
	public: virtual Object *Clone() const override { return new _className(*this); } \
	private:

// 未默认实现 Destroy 函数
#define MM_TAG_CLONEABLE_UNIMPL \
	public: virtual Object *Clone() const override; \
	private:

// 实现 Destroy
#define MM_IMPL_CLONE(_className) \
	Object *_className::Clone() const

/**
 * MM_TAG_DESTROYABLE 用以支持以基类指针清理资源特性
 */
#define MM_TAG_DESTROYABLE \
	public: virtual void Destroy() override { delete this; } \
	private:

// 未默认实现 Destroy 函数
#define MM_TAG_DESTROYABLE_UNIMPL \
	public: virtual void Destroy() override; \
	private:

// 实现 Destroy
#define MM_IMPL_DESTROY(_className) \
	void _className::Destroy()

/**
 * 默认的 Object 派生类标签
 */
#define MM_TAG_OBJECT(_className) \
	MM_TAG_CLONEABLE(_className) \
	MM_TAG_DESTROYABLE

// 注意：使用 MM_TAG_ABSTRACT 为类标注为抽象类后，需在源文件内实现类的析构函数
#define MM_TAG_ABSTRACT(_className) public: virtual ~_className() = 0; private:

MM_BEGIN

/**
 * ModernMagic 大部分组件的基类
 */
class Object : public DyPropertyClass
{
public:
	virtual Object *Clone() {
		return static_cast<const Object*>(this)->Clone();
	}
	virtual Object *Clone() const = 0;
	virtual void Destroy() = 0;

	// statics
	template <typename T>
	static T *Clone(T const *v) {
		return static_cast<T*>(v->Clone());
	}

	template <typename T>
	static std::shared_ptr<T> Clone(const std::shared_ptr<T> &v) {
		return MakeShared<T>(Clone(v.get()));
	}

	template <typename T, typename U>
	static void Clone(const std::vector<T> &from, std::vector<U> *to)
	{
		MM_ASSERT(to);

		to->resize(from.size());

		int i = 0;
		for (auto &item : from) {
			(*to)[i] = Helper::Clone(item);
			++i;
		}
	}

	template <typename T, typename U>
	static void Clone(const std::vector<T> &from, std::vector<U> *to, Timeline *tl, Action *parent)
	{
		MM_ASSERT(to);

		to->resize(from.size());

		int i = 0;
		for (auto item : from) {
			auto v = Object::Clone(item);
			(*to)[i++] = v;

			v->SetRootTimeline(tl);
			v->SetParent(parent);
		}
	}

	template <typename T>
	static std::shared_ptr<T> MakeShared() {
		return std::shared_ptr<T>(new T(), Object::DestroyObject);
	}

	template <typename T>
	static std::shared_ptr<T> MakeShared(T *ptr) {
		return std::shared_ptr<T>(ptr, Object::DestroyObject);
	}

	static void DestroyObject(Object *ptr) {
		ptr->Destroy();
	}
};

/**
 * AddInObject 的派生类支持安装/卸载功能
 */
class AddInObject : public Object
{
public:
	AddInObject();
	
	bool IsInstalled() const { return _isInstalled; }
	
	bool Install() {
		if (!_isInstalled)
			return OnInstalling() ? (_isInstalled = true) : false;
		return true;
	}
	
	void Uninstall() {
		if (_isInstalled) {
			_isInstalled = false;
			OnUninstalled();
		}
	}
	
protected:
	virtual bool OnInstalling()  { return true; }
	virtual void OnUninstalled() { }
	
private:
	bool _isInstalled;
};

// 声明重写更新函数
#define MM_TAG_OVERRIDE_UPDATE \
	public: virtual bool Update(float) override; private:

// 实现更新函数
#define MM_IMPL_UPDATE(_className, _dtName) \
	bool _className::Update(float _dtName)

/**
 * 可更新对象
 */
class UpdatableObject : public AddInObject
{
public:
	virtual bool Update(float dt) { return false; }
};

MM_END