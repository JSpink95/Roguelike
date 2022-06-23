#pragma once
#include "Core.h"
#include "Core\Object.h"

//template<typename T>
//struct TStateMachine : public Object
//{
//    TBIND_OBJECT(TStateMachine, T);
//    using StateRef = typename T::Ref;
//
//    static Ref Create()
//    {
//        return std::make_shared<TStateMachine<T>>();
//    }
//
//
//
//    StateRef activeState = nullptr;
//};