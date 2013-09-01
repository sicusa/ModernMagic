#pragma once

#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <functional>
#include <unordered_map>
#include <memory>

#define MM_BEGIN namespace ModernMagic {
#define MM_END   }

#define __MM_PROPERTY(_type, _completeType, _valueName, _proName, _getterName, _setterName) \
	private: _type _valueName; \
	public: _completeType _getterName() const { return _valueName; } \
	void  _setterName(_completeType v) { _valueName = v; }

// Unimplemented setter
#define __MM_PROPERTY_US(_type, _completeType, _valueName, _proName, _getterName, _setterName) \
	private: _type _valueName; \
	public: _completeType _getterName() const { return _valueName; } \
	void  _setterName(_completeType v); 

// No setter
#define __MM_PROPERTY_NS(_type, _completeType, _valueName, _proName, _getterName) \
	private: _type _valueName; \
	public: _completeType _getterName() const { return _valueName; }

// No getter
#define __MM_PROPERTY_NG(_type, _completeType, _valueName, _proName, _setterName) \
	private: _type _valueName; \
	public:  void _setterName(_completeType v) { _valueName = v; }


#define __MM_PROPERTY_STD(_type, _completeType, _valueName, _funcSuffix) \
	__MM_PROPERTY(_type, _completeType, _valueName, _funcSuffix, Get##_funcSuffix, Set##_funcSuffix)

#define __MM_PROPERTY_STD_US(_type, _completeType, _valueName, _funcSuffix) \
	__MM_PROPERTY_US(_type, _completeType, _valueName, _funcSuffix, Get##_funcSuffix, Set##_funcSuffix)

#define __MM_PROPERTY_STD_NS(_type, _completeType, _valueName, _funcSuffix) \
	__MM_PROPERTY_NS(_type, _completeType, _valueName, _funcSuffix, Get##_funcSuffix)

#define __MM_PROPERTY_STD_NG(_type, _completeType, _valueName, _funcSuffix) \
	__MM_PROPERTY_NG(_type, _completeType, _valueName, _funcSuffix, Set##_funcSuffix)

// Setter + Getter
#define MM_PROPERTY_PBV(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD(_type, _type, _valueName, _funcName)

#define MM_PROPERTY_PBR(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD(_type, const _type&, _valueName, _funcName)

// Unimplemented setter
#define MM_PROPERTY_PBV_US(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD_US(_type, _type, _valueName, _funcName)

#define MM_PROPERTY_PBR_US(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD_US(_type, const _type&, _valueName, _funcName)

// No setter
#define MM_PROPERTY_PBV_NS(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD_NS(_type, _type, _valueName, _funcName)

#define MM_PROPERTY_PBR_NS(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD_NS(_type, const _type&, _valueName, _funcName)

// No getter
#define MM_PROPERTY_PBV_NG(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD_NG(_type, _type, _valueName, _funcName)

#define MM_PROPERTY_PBR_NG(_type, _valueName, _funcName) \
	__MM_PROPERTY_STD_NG(_type, const _type&, _valueName, _funcName, _funcName)

#define MM_PROPERTY_BOOL(_valueName, _funcName) \
	__MM_PROPERTY(bool, bool, _valueName, _funcName, _funcName, Set##_funcName)

#define MM_PROPERTY_BOOL_US(_valueName, _funcName) \
	__MM_PROPERTY_US(bool, bool, _valueName, _funcName, _funcName, Set##_funcName)

#define MM_PROPERTY_BOOL_NS(_valueName, _funcName) \
	__MM_PROPERTY_NS(bool, bool, _valueName, _funcName, _funcName)

#define MM_PROPERTY_BOOL_NG(_valueName, _funcName) \
	__MM_PROPERTY_NG(bool, bool, _valueName, _funcName, Set##_funcName)


#define MM_PRO_BIN_OPER(_obj, _proName, _op, _value) \
	(_obj).Set##_proName((_obj).Get##_proName() _op (_value))

#define MM_PRO_BIN_OPER_PTR(_obj, _proName, _op, _value) \
	(_obj)->Set##_proName((_obj)->Get##_proName() _op (_value))


#define MM_EVENT(_type, _valueName, _funcName) \
	protected: _type _valueName; \
	public: void Set##_funcName(const _type &v) { _valueName = v; } \
	bool IsSet##_funcName() const { return _valueName != nullptr; }


#ifdef _DEBUG
#define MM_DEBUGGING
#endif

#ifdef MM_DEBUGGING
#include <assert.h>
#define MM_ASSERT(_v) assert(_v)
#else
#define MM_ASSERT(_v)
#endif

#ifdef MM_NOEXCEPTION
#define MM_THROW	if (true) {} else
#define MM_TRY		if (true)
#define MM_CATCH(e) else
#else
#define MM_THROW	throw
#define MM_TRY		try
#define MM_CATCH(e) catch(e)
#endif

#ifndef MM_BULLET_CACHE_SIZE
#define MM_BULLET_CACHE_SIZE 2000
#endif

MM_BEGIN

class IRenderer;

struct PropertyInfo;
class DyBasicProperty;
class DyPropertyClass;

class Object;
class AddInObject;

class IBodyRenderer;
class Body;
class Bullet;
class Laser;

class Action;
class Timeline;
class LoopTimeline;

class Spellbook;
class SpellbookReader;

enum
{
	kVersion_0_0_1
};

MM_END