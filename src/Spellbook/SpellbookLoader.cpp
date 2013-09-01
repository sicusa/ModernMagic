#include "stdafx.h"

#include "SpellbookLoader.h"

#include "../Timeline/Timeline.h"

#include "../ObjectCreator.h"
#include "../PropertyHelper.h"
#include "../Engine.h"
#include "../Body.h"

#include "Rapidxml/rapidxml.hpp"

#include <fstream>

using namespace rapidxml;

MM_BEGIN

namespace Detail {
	Object *LoadObject(xml_node<> *node)
	{
		MM_ASSERT(node);

		Object *obj;

		MM_TRY {
			obj = theMMObjectCreator.Create(node->name());
		}
		MM_CATCH(Exception &e) {
			MM_THROW SpellbookLoaderLoadObjectFailed (
				"SpellbookLoader[LoadObject]: 对象创建失败 -->\n    "
				+ e.str()
			);
		}

		if (obj == nullptr) {
			return nullptr;
		}

		xml_attribute<> *attr, *name = nullptr;

		for (attr = node->first_attribute();
			attr != nullptr;
			attr = attr->next_attribute())
		{
			if (!strcmp(attr->name(), "Name")) {
				name = attr;
				continue;
			}

			DyBasicProperty *pro = obj->GetProperty(attr->name());
			if (pro == nullptr) {
				obj->Destroy();

				MM_THROW SpellbookLoaderLoadObjectFailed (
					"SpellbookLoader[LoadObject]: 查找属性 '" + String(attr->name()) +
					"\' 失败"
				);
				
				return nullptr;
			}
			bool isSucc = false;

			MM_TRY {
				isSucc = theMMPropertyHelper.SetPropertyByString(pro, attr->value());
			}
			MM_CATCH (Exception &e) {
				obj->Destroy();
				MM_THROW SpellbookLoaderLoadObjectFailed (
					"SpellbookLoader[LoadObject]: 属性 '" +
					String(attr->name()) + "' 读取失败 -->\n    " +
					e.str()
				);
			}

			if (!isSucc) {
				return nullptr;
			}
		}

		if (name != nullptr) {
			theMMNamedObjects[name->value()] = obj;
		}
		return obj;
	}

	bool LoadResource(xml_node<> *node)
	{
		MM_ASSERT(node);

		return true;
	}

	template <typename T>
	bool LoadObjects(xml_node<> *node, std::vector<T*> *objs)
	{
		MM_ASSERT(node && objs);

		T *act;
		for (xml_node<> *cnode = node->first_node();
			cnode != nullptr;
			cnode = cnode->next_sibling())
		{
			MM_TRY {
				act = dynamic_cast<T*>(LoadObject(cnode));
			}
			MM_CATCH(Exception &e) {
				for (auto each : *objs) {
					each->Destroy();
				}
				MM_THROW SpellbookLoaderLoadTimelineFailed (
					"SpellbookLoader[LoadContainer]: 容器子对象 '" + String(cnode->name()) +
					"' 读取失败 -->\n    "+
					e.str()
				);
			}

			if (act == nullptr) {
				for (auto &each : *objs) {
					each->Destroy();
				}
				MM_THROW SpellbookLoaderLoadTimelineFailed (
					"SpellbookLoader[LoadContainer]: 容器子对象 '" + String(cnode->name()) +
					"' 读取失败: 该对象类型无效"
				);
				return false;
			}
			
			objs->push_back(act);
		}

		return true;
	}

	template <typename T>
	T *CreateContainer(xml_node<> *node)
	{
		MM_ASSERT(node);

		T *container;

		MM_TRY {
			container = dynamic_cast<T*>(Detail::LoadObject(node));
		}
		MM_CATCH (Exception &e) {
			MM_THROW SpellbookLoaderLoadTimelineFailed (
				"SpellbookLoader[LoadContainer]: 容器 '" +
				String(node->name()) + "' 创建失败 -->\n    "+
				e.str()
			);
		}

		if (container == nullptr) {
			MM_THROW SpellbookLoaderLoadTimelineFailed (
				"SpellbookLoader[LoadContainer]: 容器 '" +
				String(node->name()) + "' 创建失败，定义的对象并非指定的 '" +
				typeid(T).name()
			);
			return nullptr;
		}
		return container;
	}

	template <typename U, typename T>
	T *LoadContainer(xml_node<> *node, T *con = nullptr)
	{
		MM_ASSERT(node);

		T *container;

		if (con) {
			container = con;
		}
		else {
			container = CreateContainer<T>(node);
		}
		std::vector<U*> objs;

		MM_TRY {
			if (!LoadObjects(node, &objs)) {
				if (!con) {
					container->Destroy();
				}
				return nullptr;
			}
		}
		MM_CATCH (Exception &e) {
			MM_THROW SpellbookLoaderLoadTimelineFailed (
				"SpellbookLoader[LoadContainer]: Item 加载失败 -->\n    "+
				e.str()
			);
		}

		for (auto each : objs) {
			container->Add(each);
		}
		return container;
	}
}

SpellbookLoader::SpellbookLoader()
{

}

Spellbook *SpellbookLoader::LoadSpellbook(const String &sc)
{
	xml_document<> doc;
	doc.parse<0>(const_cast<char*>(sc.c_str()));

	xml_node<> *node_sb = doc.first_node("Spellbook"), *childnode = nullptr;
	
	for (xml_node<> *cnode = node_sb->first_node();
		 cnode != nullptr;
		 cnode = cnode->next_sibling())
	{
		if (!strcmp(cnode->name(), "Resource")) {
			Detail::LoadResource(cnode);
		}
		else if (!strcmp(cnode->name(), "MainTimeline")) {
			Detail::LoadContainer<Action>(cnode, &theMMMainTimeline);
		}
		else if (!strcmp(cnode->name(), "ActionUpdater")) {
			Detail::LoadContainer<Action>(cnode, &theMMActionUpdater);
		}
		else if (!strcmp(cnode->name(), "BodyUpdater")) {
			Detail::LoadContainer<Body>(cnode, &theMMBodyUpdater);
		}
	}

	return nullptr;
}

Spellbook *SpellbookLoader::LoadSpellbookFromFile(const String &path)
{
	std::fstream scfile(path);
	if (!scfile) {
		return nullptr;
	}
	scfile.seekg(0, std::ios::end);
	size_t size = scfile.tellg();
	scfile.seekg(0, std::ios::beg);

	String sc;
	sc.resize(size);
	scfile.read(&sc[0], size);

	return LoadSpellbook(sc);
}

Timeline *SpellbookLoader::LoadTimeline(const String &sc)
{
	xml_document<> doc;
	doc.parse<0>(const_cast<char*>(sc.c_str()));

	return Detail::LoadContainer<Action, Timeline>(doc.first_node());
}

Object *SpellbookLoader::LoadObject(const String &sc)
{
	xml_document<> doc;
	doc.parse<0>(const_cast<char*>(sc.c_str()));

	return Detail::LoadObject(doc.first_node());
}

bool SpellbookLoader::LoadResource(const String &sc)
{
	xml_document<> doc;
	doc.parse<0>(const_cast<char*>(sc.c_str()));

	return Detail::LoadResource(doc.first_node());
}

MM_END