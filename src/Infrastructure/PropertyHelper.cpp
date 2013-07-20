#include "stdafx.h"

#include "PropertyHelper.h"
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
		/*
	// Color
	MM_REG_PRO_PARSER_BEGIN(Color, pro, str)
		StringList split = SplitString(str);
		Vector2 retVal;

		if( split.size() > 0 )
			retVal.X = StringToFloat(split[0]);

		if( split.size() > 1 )
			retVal.Y = StringToFloat(split[1]);

		return retVal;
	MM_REG_PRO_PARSER_END*/
}

void PropertyHelper::RegisterParser(size_t hash, const ParserFunc &parser)
{
	_pfuncs[hash] = parser;
}

bool PropertyHelper::GetPropertyFromString(DyBasicProperty *pro, const String &str)
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

bool PropertyHelper::GetPropertyFromString(Object *obj, const String &proname, const String &str)
{
	auto pro = obj->GetProperty(proname);
	if (pro == nullptr) {
		return false;
	}
	return this->GetPropertyFromString(pro, str);
}

MM_END