#pragma once
#include "Core.h"
#include "Core\typeid.h"

#define BIND_OBJECT(obj)            \
BIND_TYPE(obj)                      \
using Super = __super::ThisType;    \
inline Ref Get##obj##Ref()\
{\
    return std::dynamic_pointer_cast<obj>(shared_from_this());\
}\
inline static Ref Cast(Object::Ref object)\
{\
    return object ? std::dynamic_pointer_cast<obj>(object) : nullptr;\
}

#define TBIND_OBJECT(obj, tem)      \
BIND_TYPE(obj<tem>)                 \
inline Ref Get##obj##Ref()\
{\
    return std::dynamic_pointer_cast<ThisType>(shared_from_this());\
}\
inline static Ref Cast(Object::Ref object)\
{\
    return object ? std::dynamic_pointer_cast<ThisType>(object) : nullptr;\
}

struct Object
    : public core::ITypeIDRegister
    , public std::enable_shared_from_this<Object>
{
    BIND_OBJECT(Object);
    virtual ~Object() = default;
};
