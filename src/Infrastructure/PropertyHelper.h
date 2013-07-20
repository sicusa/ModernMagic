#pragma once

#include "../Util/StringUtil.h"

#include <sstream>

#define theMMPropertyHelper ::ModernMagic::PropertyHelper::GetInstance()

#define MM_REG_PRO_PARSER_BEGIN(t, pro, str)\
	theMMPropertyHelper.RegisterParser<t>([](DyBasicProperty *pro, const String &str) -> bool {

#define MM_REG_PRO_PARSER_END });

MM_BEGIN

class PropertyHelper
{
public:
	// 将数据从 String 读取，保存到指定属性中的解析器
	// 返回是否解析并保存成功
	typedef std::function<bool(DyBasicProperty*, const String&)>	ParserFunc;
	typedef std::unordered_map<size_t, ParserFunc>					ParserFuncContainer;

public:
	static PropertyHelper &GetInstance() {
		static PropertyHelper helper;
		return helper;
	}

	/**
	 * 注册一个 String 到 Property 的解析器
	 *
	 * @param 解析器针对的属性类型的哈希值
	 * @param 解析器函数
	 */
	void RegisterParser(size_t hash, const ParserFunc &parser);
	template <typename T>
	void RegisterParser(const ParserFunc &parser) {
		this->RegisterParser(typeid(T).hash_code(), parser);
	}

	/**
	 * 使用 stringstream 的通用解析器注册函数
	 */
	template <typename T>
	void RegisterParser() {
		this->RegisterParser<T>([](DyBasicProperty *pro, const String &str) -> bool {
			using namespace std;
			
			T tmp;
			std::istringstream ss(str);
			ss >> tmp;

			if (ss.fail()) {
				return false;
			}

			// GetPropertyFromString 会检测类型与权限是否匹配
			// 因此这里直接使用 UnsafeSet
			pro->UnsafeSet<T>(tmp);
			return true;
		});
	}

	/**
	 * 将 str 中的数据转换读取到指定的属性中
	 *
	 * @return 是否读取成功
	 */
	bool GetPropertyFromString(DyBasicProperty *pro, const String &str);
	bool GetPropertyFromString(Object *obj, const String &proname, const String &str);

private:
	PropertyHelper();

	ParserFuncContainer _pfuncs;
};

MM_END