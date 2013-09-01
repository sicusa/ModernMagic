#include "stdafx.h"

#include "PropertyHelper.h"

#include "DynamicProperty.h"
#include "Object.h"
#include "Exceptions.h"
#include "Engine.h"

#include "Spellbook/SpellbookLoader.h"

#include "Infrastructure/Color.h"
#include "Infrastructure/Interval.h"

MM_BEGIN

PropertyHelper::PropertyHelper()
{
	/************************************************************************/
	/* Parsers                                                              */
	/************************************************************************/
	this->RegisterParser<short>();
	this->RegisterParser<int>();
	this->RegisterParser<long>();
	this->RegisterParser<long long>();
	this->RegisterParser<char>();

	this->RegisterParser<unsigned short>();
	this->RegisterParser<unsigned int>();
	this->RegisterParser<unsigned long>();
	this->RegisterParser<unsigned long long>();
	this->RegisterParser<unsigned char>();

	this->RegisterParser<float>();
	this->RegisterParser<double>();
	this->RegisterParser<long double>();

	// String
	MM_REG_STRTOPRO_PARSER_BEGIN(String, pro, str)
		pro->UnsafeSet<String>(str);
		return true;
	MM_REG_PARSER_END

	MM_REG_PROTOSTR_PARSER_BEGIN(String, str, pro)
		str = pro->UnsafeGet<String>();
		return true;
	MM_REG_PARSER_END

	// bool
	MM_REG_STRTOPRO_PARSER_BEGIN(bool, pro, str)
		if (str == "true") {
			pro->UnsafeSet<bool>(true);
			return true;
		}
		else if (str == "false") {
			pro->UnsafeSet<bool>(false);
			return true;
		}
		return false;
	MM_REG_PARSER_END

	MM_REG_PROTOSTR_PARSER_BEGIN(bool, str, pro)
		str = pro->UnsafeGet<bool>() ? "true" : "false";
		return true;
	MM_REG_PARSER_END

	// Vector2
	MM_REG_STRTOPRO_PARSER_BEGIN(Vector2, pro, str)
		pro->UnsafeSet<Vector2>(StringToVector2(str));
		return true;
	MM_REG_PARSER_END

	MM_REG_PROTOSTR_PARSER_BEGIN(Vector2, str, pro)
		str = Vector2ToString(pro->UnsafeGet<Vector2>());
		return true;
	MM_REG_PARSER_END

	// Color
	MM_REG_STRTOPRO_PARSER_BEGIN(Color, pro, str)
		if (str[0] == '#') {
			pro->UnsafeSet<Color>(Color::FromHexString(str));
			return true;
		}

		StringList split = SplitString(str, ",");
		if (split.size() != 3) {
			return false;
		}

		Color color;
		std::stringstream ss;
		char c;

		ss << split[0] << split[1] << split[2];

		ss >> c;
		color.R = c / 255.0f;

		ss >> c;
		color.G = c / 255.0f;

		ss >> c;
		color.B = c / 255.0f;

		if (ss.fail()) {
			return false;
		}

		pro->UnsafeSet<Color>(color);
		return true;
	MM_REG_PARSER_END

	MM_REG_PROTOSTR_PARSER_BEGIN(Color, str, pro)
		const Color c = pro->UnsafeGet<Color>();
		str = std::to_string((char)c.R * 255) + ',' +
			  std::to_string((char)c.G * 255) + ',' +
			  std::to_string((char)c.B * 255);
		return true;
	MM_REG_PARSER_END

	/************************************************************************/
	/* Tags                                                                 */
	/************************************************************************/
	// Colors
	this->RegisterTag("White",		Colors::White);
	this->RegisterTag("Black",		Colors::Black);
	this->RegisterTag("Red",			Colors::Red);
	this->RegisterTag("Green",		Colors::Green);
	this->RegisterTag("Blue",		Colors::Blue);
	this->RegisterTag("Yellow",		Colors::Yellow);
	this->RegisterTag("Cyan",		Colors::Cyan);
	this->RegisterTag("Magenta",		Colors::Magenta);
	this->RegisterTag("BeachSand",	Colors::BeachSand);
	this->RegisterTag("DesertSand",	Colors::DesertSand);
	this->RegisterTag("LightGreen",	Colors::LightGreen);
	this->RegisterTag("PureGreen",	Colors::PureGreen);
	this->RegisterTag("DarkGreen",	Colors::DarkGreen);
	this->RegisterTag("LightYellowGreen",	Colors::LightYellowGreen);
	this->RegisterTag("PureYellowGreen",		Colors::PureYellowGreen);
	this->RegisterTag("DarkYellowGreen",		Colors::DarkYellowGreen);
	this->RegisterTag("LightBrown",	Colors::LightBrown);
	this->RegisterTag("DarkBrown",	Colors::DarkBrown);

	// Vector2
	this->RegisterTag<Vector2>("Center", [](DyBasicProperty *pro) {
		const BoundingBox &box = theMMEngine.GetWorldBox();
		Vector2 size = box.Max + box.Min;
		pro->UnsafeSet<Vector2>(size / 2.0f);
	});
	this->RegisterTag("UnitX", Vector2::UnitX);
	this->RegisterTag("UnitY", Vector2::UnitY);
	this->RegisterTag("Zero", Vector2::Zero);
	this->RegisterTag("One", Vector2::One);

	// IntervalFunc
	this->RegisterTag<IntervalFunc>("Linear", IF_Linear);
	this->RegisterTag<IntervalFunc>("Smooth", IF_Smooth);
	this->RegisterTag<IntervalFunc>("SineIn", IF_SineIn);
	this->RegisterTag<IntervalFunc>("SineOut", IF_SineOut);
	this->RegisterTag<IntervalFunc>("SineInOut", IF_SineInOut);
	this->RegisterTag<IntervalFunc>("ExponentialIn", IF_ExponentialIn);
	this->RegisterTag<IntervalFunc>("ExponentialOut", IF_ExponentialOut);
	this->RegisterTag<IntervalFunc>("ExponentialInOut", IF_ExponentialInOut);
}

bool PropertyHelper::SetPropertyByString(DyBasicProperty *pro, const String &str) const
{
	MM_ASSERT(pro);

	if (pro->GetPermission() == PropertyPermission::Readonly) {
		MM_THROW PropertyWriteFailed(
			"PropertyHelper[SetPropertyByString]: 属性 '" + pro->GetName() +
			"' 无法修改"
		);
		return false;
	}

	auto iter_tag = _tags.find(str);
	if (iter_tag != _tags.end()
			&& iter_tag->second.Hash == pro->GetHash()) {
		iter_tag->second.Setter(pro);
		return true;
	}

	auto iter_pf = _stpFuncs.find(pro->GetHash());
	if (iter_pf == _stpFuncs.end()) {
		MM_THROW PropertyHelperParserNotFound (
			"PropertyHelper[SetPropertyByString]: 没有与属性 '" + pro->GetName() +
			"' 匹配的解析器"
		);
		return false;
	}
	if (!iter_pf->second(pro, str)) {
		MM_THROW PropertyHelperParserError (
			"PropertyHelper[SetPropertyByString]: 字符串信息 [" + str +
			"] 无法被读取到属性 '" + pro->GetName() + "' 中，Parser 解析错误"
		);
		return false;
	}
	return true;
}

bool PropertyHelper::SetPropertyByString(Object *obj, const String &proname, const String &str) const
{
	MM_ASSERT(obj);

	auto pro = obj->GetProperty(proname);
	if (pro == nullptr) {
		MM_THROW PropertyNotFound (
			"PropertyHelper[SetPropertyByString]: 没有在对象中找到属性 '" + proname +
			'\''
		);
		return false;
	}
	return this->SetPropertyByString(pro, str);
}

bool PropertyHelper::ToString(String &str, const DyBasicProperty *pro) const
{
	MM_ASSERT(pro);

	if (pro->GetPermission() == PropertyPermission::Writeonly) {
		MM_THROW PropertyWriteFailed(
			"PropertyHelper[ToString]: 属性 '" + pro->GetName() +
			"' 无法读取"
		);
		return false;
	}

	auto iter_pf = _ptsFuncs.find(pro->GetHash());
	if (iter_pf == _ptsFuncs.end()) {
		MM_THROW PropertyHelperParserNotFound (
			"PropertyHelper[SetPropertyByString]: 没有与属性 '" + pro->GetName() +
			"' 匹配的解析器"
		);
		return false;
	}
	if (!iter_pf->second(str, pro)) {
		MM_THROW PropertyHelperParserError (
			"PropertyHelper[SetPropertyByString]: 属性 '" +
			pro->GetName() + "' 转换为字符串失败，Parser 解析错误"
		);
		return false;
	}
	return true;
}

bool PropertyHelper::ToString(String &str, Object *obj, const String &proname) const
{
	MM_ASSERT(obj);

	auto pro = obj->GetProperty(proname);
	if (pro == nullptr) {
		MM_THROW PropertyNotFound (
			"PropertyHelper[SetPropertyByString]: 没有在对象中找到属性 '" + proname +
			'\''
		);
		return false;
	}
	return this->ToString(str, pro);
}

void PropertyHelper::RegisterTag(const String &name, size_t hash, const TagPropertySetter &setter)
{
	MM_ASSERT(hash);

	Tag &tag = _tags[name];
	tag.Hash = hash;
	tag.Setter = setter;
}

void PropertyHelper::RegisterParserProToStr(size_t hash, const ParserFuncProToStr &parser)
{
	MM_ASSERT(hash);

	_ptsFuncs[hash] = parser;
}

void PropertyHelper::RegisterParserStrToPro(size_t hash, const ParserFuncStrToPro &parser)
{
	MM_ASSERT(hash);

	_stpFuncs[hash] = parser;
}

MM_END