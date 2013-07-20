#include "stdafx.h"

#include "Exceptions.h"

MM_BEGIN

Exception::Exception(const String &msg) throw():
	_message(msg)
{
}

MM_END
