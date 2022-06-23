#pragma once
#include "Core.h"

namespace core
{
    struct TypeIDGenerator
    {
        static inline u64 id = 0u;
        static u64 next()
        {
            return ++id;
        }
    };

    template<typename T> struct TypeID
    {
        static u64 get()
        {
            static const u64 TypeID = TypeIDGenerator::next();
            return TypeID;
        }
    };

    // base class, use the macro helper for objects to implement at specific levels
    struct ITypeIDRegister
    {
        using ThisType = ITypeIDRegister;
        virtual u64 GetTypeID() const = 0;
    };
}

#define BIND_TYPE(type)                                             \
using ThisType = type;                                              \
using Ref = std::shared_ptr<type>;                                  \
using WeakRef = std::weak_ptr<type>;                                \
static inline u64 ThisTypeID = core::TypeID<##type##>::get();       \
u64 GetTypeID() const override                                      \
{                                                                   \
    return ThisTypeID;                                              \
}