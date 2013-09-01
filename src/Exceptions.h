#pragma once

#include "Util/StringUtil.h"

#include <exception>

#define MM_DEF_EXCEPTION(_className, _subclass) \
	class _className : public _subclass { \
	public: \
		_className (const String &msg="") : _subclass(msg) {} \
	};

#define MM_DEF_BASIC_EXCEPTION(_className) MM_DEF_EXCEPTION(_className, ::ModernMagic::Exception)

MM_BEGIN

class Exception : public std::exception
{
public:
	Exception(const String &msg) throw();

	const char* what() const throw() override {
		return _message.c_str();
	}

	const String &str() const throw() {
		return _message;
	}

private:
	String _message;
};

MM_DEF_BASIC_EXCEPTION(InstallingFailed);

MM_DEF_BASIC_EXCEPTION(UnimplementedClone);
MM_DEF_BASIC_EXCEPTION(UnimplementedDestroy);

MM_DEF_BASIC_EXCEPTION(UnknownBodyType);

MM_DEF_BASIC_EXCEPTION(PropertyCaseFailed)
MM_DEF_BASIC_EXCEPTION(PropertyReadFailed)
MM_DEF_BASIC_EXCEPTION(PropertyWriteFailed)
MM_DEF_BASIC_EXCEPTION(PropertyNotFound)
MM_DEF_BASIC_EXCEPTION(PropertyHelperParserError)
MM_DEF_BASIC_EXCEPTION(PropertyHelperParserNotFound)

MM_DEF_BASIC_EXCEPTION(ClassFactoryCreateObjectFailed)

MM_DEF_BASIC_EXCEPTION(SpellbookLoaderLoadObjectFailed)
MM_DEF_BASIC_EXCEPTION(SpellbookLoaderLoadTimelineFailed)
MM_DEF_BASIC_EXCEPTION(SpellbookLoaderLoadSpellbookFailed)

MM_END