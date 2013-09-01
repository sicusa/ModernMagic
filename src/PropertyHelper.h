#pragma once

#include "Util/StringUtil.h"

#include <sstream>

#define theMMPropertyHelper ::ModernMagic::PropertyHelper::GetInstance()

#define MM_REG_PROTOSTR_PARSER_BEGIN(t, str, pro) \
	theMMPropertyHelper.RegisterParserProToStr<t>([](String &str, const DyBasicProperty *pro) -> bool {


#define MM_REG_STRTOPRO_PARSER_BEGIN(t, pro, str) \
	theMMPropertyHelper.RegisterParserStrToPro<t>([](DyBasicProperty *pro, const String &str) -> bool {

#define MM_REG_PARSER_END });

MM_BEGIN

class PropertyHelper
{
public:
	// 将数据从 String 读取，保存到指定属性中的解析器
	typedef std::function<bool(String&, const DyBasicProperty*)> ParserFuncProToStr;
	// 将属性数据转换为字符串
	typedef std::function<bool(DyBasicProperty*, const String&)> ParserFuncStrToPro;

	typedef std::unordered_map<size_t, ParserFuncProToStr> ParserFuncProToStrContainer;
	typedef std::unordered_map<size_t, ParserFuncStrToPro> ParserFuncStrToProContainer;

	// 标签属性设置函数
	typedef std::function<void(DyBasicProperty*)> TagPropertySetter;

	// 标签（允许指定某一类型的某个值的代替字符串）
	struct Tag
	{
		// 标签对应的类型
		size_t Hash;

		// 属性设置函数
		TagPropertySetter Setter;
	};

	typedef std::unordered_map<String, Tag> TagContainer;

public:
	static PropertyHelper &GetInstance() {
		static PropertyHelper helper;
		return helper;
	}

	/**
	 * 将 str 中的数据转换读取到指定的属性中
	 *
	 * @return 是否读取成功
	 */
	bool SetPropertyByString(DyBasicProperty *pro, const String &str) const;
	bool SetPropertyByString(Object *obj, const String &proname, const String &str) const;

	/**
	 * 将属性中的数据读取转换成字符串
	 *
	 * @return 是否读取成功
	 */
	bool ToString(String &str, const DyBasicProperty *pro) const;
	bool ToString(String &str, Object *obj, const String &proname) const;

	/**
	 * 注册一个字符串转属性解析器
	 *
	 * @param 解析器针对的属性类型的哈希值
	 * @param 解析器函数
	 */
	template <typename T>
	void RegisterParserStrToPro(const ParserFuncStrToPro &parser) {
		this->RegisterParserStrToPro(typeid(T).hash_code(), parser);
	}

	/**
	 * 通用字符串转属性解析器注册函数
	 */
	template <typename T>
	void RegisterParserStrToPro() {
		this->RegisterParserStrToPro<T>([](DyBasicProperty *pro, const String &str) -> bool {
			T tmp;
			std::istringstream ss(str);
			ss >> tmp;

			if (ss.fail()) {
				return false;
			}

			// RegisterParserStrToPro 会检测属性权限是否有效
			// 因此这里直接使用 UnsafeSet
			pro->UnsafeSet<T>(tmp);
			return true;
		});
	}

	/**
	 * 获取所有的字符串转属性解析器
	 */
	const ParserFuncStrToProContainer &GetParsersStrToPro() const {
		return _stpFuncs;
	}

	/**
	 * 注册一个 String 到 Property 的解析器
	 *
	 * @param 解析器针对的属性类型的哈希值
	 * @param 解析器函数
	 */
	template <typename T>
	void RegisterParserProToStr(const ParserFuncProToStr &parser) {
		this->RegisterParserProToStr(typeid(T).hash_code(), parser);
	}

	/**
	 * 通用属性转字符串解析器注册函数
	 */
	template <typename T>
	void RegisterParserProToStr() {
		this->RegisterParserProToStr<T>([](String &str, const DyBasicProperty *pro) -> bool {
			str = std::to_string(pro->UnsafeGet<T>());
			return true;
		});
	}

	/**
	 * 获取所有的属性转字符串解析器
	 */
	const ParserFuncProToStrContainer &GetParsersProToStr() const {
		return _ptsFuncs;
	}

	/**
	 * 注册双向转换解释器
	 */
	template <typename T>
	void RegisterParser() {
		this->RegisterParserProToStr<T>();
		this->RegisterParserStrToPro<T>();
	}
	template <typename T>
	void RegisterParser(const ParserFuncProToStr &pts_parser, const ParserFuncStrToPro &stp_parser) {
		this->RegisterParserProToStr<T>(pts_parser);
		this->RegisterParserStrToPro<T>(stp_parser);
	}

	/**
	 * 注册标签
	 *
	 * @param 标签名
	 * @param 标签的设置器
	 */
	template <typename T>
	void RegisterTag(const String &name, const TagPropertySetter &setter) {
		this->RegisterTag(name, typeid(T).hash_code(), setter);
	}

	/**
	 * 注册标签
	 *
	 * @param 标签名
	 * @param 标签的值
	 */
	template <typename T>
	void RegisterTag(const String &name, const T &value) {
		this->RegisterTag(name, typeid(T).hash_code(), [value](DyBasicProperty *pro) {
			pro->UnsafeSet(value);
		});
	}

	/**
	 * 获取所有的标签
	 */
	const TagContainer &GetTags() const {
		return _tags;
	}

protected:
	void RegisterParserProToStr(size_t hash, const ParserFuncProToStr &parser);
	void RegisterParserStrToPro(size_t hash, const ParserFuncStrToPro &parser);
	void RegisterTag(const String &name, size_t hash, const TagPropertySetter &setter);

private:
	PropertyHelper();

	ParserFuncProToStrContainer _ptsFuncs;
	ParserFuncStrToProContainer _stpFuncs;
	TagContainer _tags;
};

MM_END