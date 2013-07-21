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

MM_DEF_BASIC_EXCEPTION(InstallingError);
MM_DEF_BASIC_EXCEPTION(UnimplementedCloneException);
MM_DEF_BASIC_EXCEPTION(UnimplementedDestroyException);
MM_DEF_BASIC_EXCEPTION(UnknownBodyType);
MM_DEF_BASIC_EXCEPTION(DyPropertyCaseException)
MM_DEF_BASIC_EXCEPTION(DyPropertyReadException)
MM_DEF_BASIC_EXCEPTION(DyPropertyWriteException)
MM_DEF_BASIC_EXCEPTION(UnregisteredBodyType)

MM_END