// Copyright © 2026 CCP ehf.


#pragma once
#ifndef BlueMemberFunctionTraits_h
#define BlueMemberFunctionTraits_h

#include "BlueFunctionTraits.h"

//////////////////////////////////////////////////////////////////////////
// Member function type traits for Blue2.0 function exposure
//////////////////////////////////////////////////////////////////////////


template<typename Function> struct function_traits;

template< typename R, typename C>
struct function_traits< R (C::*)()>
{
	typedef R return_type;
	typedef C class_type;
	typedef arity0Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C>
struct function_traits< R (C::*)() const>
{
	typedef R return_type;
	typedef C class_type;
	typedef arity0Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1 >
struct function_traits< R (C::*)(A1)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef arity1Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1 >
struct function_traits< R (C::*)(A1) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef arity1Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2 >
struct function_traits< R (C::*)(A1, A2)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef arity2Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2 >
struct function_traits< R (C::*)(A1, A2) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef arity2Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3 >
struct function_traits< R (C::*)(A1, A2, A3)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef arity3Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3 >
struct function_traits< R (C::*)(A1, A2, A3) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef arity3Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4 >
struct function_traits< R (C::*)(A1, A2, A3, A4)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef arity4Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4 >
struct function_traits< R (C::*)(A1, A2, A3, A4) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef arity4Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef arity5Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef arity5Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef arity6Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef arity6Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6, A7)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef A7 arg7_type;
	typedef arity7Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6, A7) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef A7 arg7_type;
	typedef arity7Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6, A7, A8) >
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef A7 arg7_type;
	typedef A8 arg8_type;
	typedef arity8Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6, A7, A8) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef A7 arg7_type;
	typedef A8 arg8_type;
	typedef arity8Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9)>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef A7 arg7_type;
	typedef A8 arg8_type;
	typedef A9 arg9_type;
	typedef arity9Type arity_type;
	static const int numOptional = 0;
};

template< typename R, typename C, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9 >
struct function_traits< R (C::*)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const>
{
	typedef R return_type;
	typedef C class_type;
	typedef A1 arg1_type;
	typedef A2 arg2_type;
	typedef A3 arg3_type;
	typedef A4 arg4_type;
	typedef A5 arg5_type;
	typedef A6 arg6_type;
	typedef A7 arg7_type;
	typedef A8 arg8_type;
	typedef A9 arg9_type;
	typedef arity9Type arity_type;
	static const int numOptional = 0;
};


#endif // BlueMemberFunctionTraits_h