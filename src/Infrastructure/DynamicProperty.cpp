#include "stdafx.h"

#include "DynamicProperty.h"

MM_BEGIN

DyBasicProperty::DyBasicProperty(const PropertyInfo *info):
    _info(info)
{
}

DyPropertyClass::PropertyInfoContainer DyPropertyClass::_proinfos;

DyPropertyClass::DyPropertyClass()
{
}

DyPropertyClass::DyPropertyClass(const DyPropertyClass &other)
{
}

PropertyInfo *DyPropertyClass::GetPropertyInfo(const String &name)
{
    auto iter = _proinfos.find(name);
    if (iter == _proinfos.end())
    {
        struct PropertyInfo *info = new struct PropertyInfo();
        _proinfos[name].reset(info);
        info->Name = name;
        return info;
    }
    else {
        return iter->second.get();
    }
}

MM_END