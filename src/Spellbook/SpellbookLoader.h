#pragma once

#include "../Util/StringUtil.h"

#define theMMSpellbookLoader ::ModernMagic::SpellbookLoader::GetInstance()

MM_BEGIN

class SpellbookLoader
{
public:
	static SpellbookLoader &GetInstance() {
		static SpellbookLoader reader;
		return reader;
	}
	
	/**
	 * 装载 Spellbook 脚本
	 *
	 * @param 脚本的源代码 
	 * @reutrn 读取失败返回 nullptr
	 */
	Spellbook *LoadSpellbook(const String &sc);

	/**
	 * 从文件加载 Spellbook 脚本
	 *
	 * @param 脚本路径
	 * @reutrn 读取失败返回 nullptr
	 */
	Spellbook *LoadSpellbookFromFile(const String &path);

	/**
	 * 装载一段描述时间轴的脚本
	 *
	 * @param 脚本的源代码
	 * @reutrn 读取失败返回 nullptr
	 */
	Timeline *LoadTimeline(const String &sc);

	/**
	 * 读取一段 XML 格式的对象说明脚本，创建对象
	 *
	 * @param 脚本的源代码
	 * @return 读取失败返回 nullptr
	 */
	Object *LoadObject(const String &sc);

	/**
	 * 读取一段 XML 格式的资源说明脚本，加载资源
	 *
	 * @param 脚本的源代码
	 * @return 读取失败返回 false
	 */
	bool LoadResource(const String &sc);

	template <typename T>
	T *LoadObject(const String &sc) {
		unique_ptr<T> ptr(dynamic_cast<T*>(this->LoadObject(sc)));
		if (!ptr) {
			MM_THROW SpellbookLoaderLoadObjectFailed (
				"SpellbookLoader[LoadObject]: 创建的对象无法作为指定的类型 '"
				+ typeid(T).name() + '\''
			);
			return nullptr;
		}
		return ptr.release();
	}

private:
	SpellbookLoader();

	std::unordered_map<String, Object*> _namedObject;
};

MM_END