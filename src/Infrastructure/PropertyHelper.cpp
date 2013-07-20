#include "stdafx.h"

#include "PropertyHelper.h"
#include "Color.h"
#include "../Object.h"

MM_BEGIN

PropertyHelper::PropertyHelper()
{
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
	MM_REG_PRO_PARSER_BEGIN(bool, pro, str)
		pro->UnsafeSet<String>(str);
		return true;
	MM_REG_PRO_PARSER_END

	// bool
	MM_REG_PRO_PARSER_BEGIN(bool, pro, str)
		if (str == "true") {
			pro->UnsafeSet<bool>(true);
			return true;
		}
		else if (str == "false") {
			pro->UnsafeSet<bool>(false);
			return true;
		}
		return false;
	MM_REG_PRO_PARSER_END

	// Vector2
	MM_REG_PRO_PARSER_BEGIN(Vector2, pro, str)
		pro->UnsafeSet<Vector2>(StringToVector2(str));
		return true;
	MM_REG_PRO_PARSER_END
	
	// Color
	MM_REG_PRO_PARSER_BEGIN(Color, pro, str)
		if (str[0] == '#') {
			pro->UnsafeSet<Color>(Color::FromHexString(str));
			return true;
		}

		StringList split = SplitString(str);
		Color color;

		if( split.size() > 0 )
			color.R = StringToInt(split[0]) / 255.0f;

		if( split.size() > 1 )
			color.G = StringToInt(split[1]) / 255.0f;

		if( split.size() > 2 )
			color.B = StringToInt(split[2]) / 255.0f;

		pro->UnsafeSet<Color>(color);
		return true;
	MM_REG_PRO_PARSER_END
}

void PropertyHelper::RegisterParser(size_t hash, const ParserFunc &parser)
{
	_pfuncs[hash] = parser;
}

bool PropertyHelper::SetPropertyByString(DyBasicProperty *pro, const String &str)
{
	if (pro->GetPermission() == PropertyPermission::Readonly) {
		return false;
	}
	auto iter = _pfuncs.find(pro->GetHash());
	if (iter == _pfuncs.end()) {
		return false;
	}
	return iter->second(pro, str);
}

bool PropertyHelper::SetPropertyByString(Object *obj, const String &proname, const String &str)
{
	auto pro = obj->GetProperty(proname);
	if (pro == nullptr) {
		return false;
	}
	return this->SetPropertyByString(pro, str);
}

MM_END