// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueCallFunction_h
#define BlueCallFunction_h

#include "BlueScriptValue.h"
#include "BlueExtractArgument.h"
#include "BlueExtractArgument.h"
#include "BlueCallFunctionHelpers.h"
#include "BlueMemberFunctionTraits.h"
#include "BlueMemberFunctionAsCallable.h"
#include "BlueFunctionTraits.h"


#define EXTRACT_ARGUMENT_N( args, ix ) \
	typedef typename function_traits<fnType>::arg ## ix ## _type A ## ix; \
	typedef typename remap_wchar_to_string<typename remove_const_and_reference<A ## ix>::type>::type A ## ix ## type; \
	A ## ix ## type arg ## ix; \
	if( !BlueExtractArgumentOrNullValue<fnType, A ## ix ## type>( method, args, ix, arg ## ix, typename std::conditional< (bool)function_traits<fnType>::numOptional, std::true_type, std::false_type >::type() ) ) \
	{ \
		return false; /* Error set by BlueExtractArgument */ \
	}

#include "BlueCallFunctionWithBeResult.h"

//////////////////////////////////////////////////////////////////////////
// BlueCallFunction
//
// This function does the work of constructing a function of the correct
// argument length.
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// VOID Arity 0 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity0Type a)
{
	if( !CheckArgumentLength( args, 0, 0 ) )
	{
		return false;
	}

	// call the function
	method();
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 0 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity0Type a )
{
	if( !CheckArgumentLength( args, 0, 0 ) )
	{
		return false;
	}

	returnValueRef = method();
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 1 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity1Type a)
{
	if( !CheckArgumentLength( args, 1, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );

	// call the function
	method( arg1 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 1 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity1Type a )
{
	if( !CheckArgumentLength( args, 1, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );

	// call the function
	returnValueRef = method( arg1 );
	return true;
};


//////////////////////////////////////////////////////////////////////////
// VOID Arity 1 member function for Setter Properties
//////////////////////////////////////////////////////////////////////////
// The difference in this function is that it does not try to extract the argument
// from a tuple.
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename C >
bool BlueCallPropertySetter( fnType method, C* o, BlueScriptValue val, std::true_type isBeResult )
{
	typedef typename function_traits<fnType>::return_type ResultType;
	typedef typename function_traits<fnType>::arg1_type A1;

	typename remove_const_and_reference<A1>::type arg1;
	if( !BlueExtractArgument( val, arg1, 2 ) )
	{
		return false; // Error set by BlueExtractArgument
	}

	// call the function
	ResultType result = (o->*method)( arg1 );

	if( BeIsSuccess( result ) )
	{
		return true;
	}
	else
	{
#if BLUE_WITH_PYTHON
		PyErr_SetString( BeGetException( result ), BeGetErrorMessage( result ) );
#endif
		return false;
	}
};

template< typename fnType, typename C >
bool BlueCallPropertySetter( fnType method, C* o, BlueScriptValue val, std::false_type isBeResult )
{
	typedef typename function_traits<fnType>::arg1_type A1;

	typename remove_const_and_reference<A1>::type arg1;
	if( !BlueExtractArgument( val, arg1, 2 ) )
	{
		return false; // Error set by BlueExtractArgument
	}

	// call the function
	(o->*method)( arg1 );

	return true;
};

template< typename fnType, typename C >
BlueScriptValue BlueCallPropertyGetter( fnType method, C* o, BlueScriptArguments args, std::true_type isBeResult )
{
	typedef typename function_traits<fnType>::return_type ResultType;
	typedef typename function_traits<fnType>::arg1_type A1;
	static_assert( std::is_reference<A1>::value, "Getters returning Be::Result must have one argument, as a reference" );

	typename remove_const_and_reference<A1>::type arg1;

	// call the function
	ResultType result = (o->*method)( arg1 );

	if( BeIsSuccess( result ) )
	{
		return BlueGetReturnValueIfSuccess( args, result, arg1 );
	}
	else
	{
#if BLUE_WITH_PYTHON
		PyErr_SetString( BeGetException( result ), BeGetErrorMessage( result ) );
#endif
		return BLUE_SCRIPT_ERROR;
	}
};

template< typename fnType, typename C >
BlueScriptValue BlueCallPropertyGetter( fnType method, C* o, BlueScriptArguments args, std::false_type isBeResult )
{
	static_assert( std::is_same<typename function_traits<fnType>::arity_type, arity0Type>::value, "Getter function can't take any arguments" );

	typedef 
		typename function_traits<fnType>::return_type 
		ReturnType;

	typedef 
		typename remove_const_and_reference<ReturnType>::type 
		StrippedReturnType;

	BlueMemberFunctionAsCallable<fnType> callable( o, method );

	StrippedReturnType r;
	if( BlueCallFunction( callable, args, r, arity0Type() ) )
	{
		return BlueWrapReturnValue( args, r );
	}
	else
	{
		return BLUE_SCRIPT_ERROR;
	}
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 2 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity2Type a)
{
	if( !CheckArgumentLength( args, 2, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );

	// call the function
	method( arg1, arg2 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 2 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity2Type a )
{
	if( !CheckArgumentLength( args, 2, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );

	// call the function
	returnValueRef = method( arg1, arg2 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 3 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity3Type a)
{
	if( !CheckArgumentLength( args, 3, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );

	// call the function
	method( arg1, arg2, arg3 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 3 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity3Type a )
{
	if( !CheckArgumentLength( args, 3, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );

	// call the function
	returnValueRef = method( arg1, arg2, arg3 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 4 member function
//////////////////////////////////////////////////////////////////////////
template<  typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity4Type a )
{
	if( !CheckArgumentLength( args, 4, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );

	// call the function
	method( arg1, arg2, arg3, arg4 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 4 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity4Type a )
{
	if( !CheckArgumentLength( args, 4, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );

	// call the function
	returnValueRef = method( arg1, arg2, arg3, arg4 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 5 member function
//////////////////////////////////////////////////////////////////////////
template<  typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity5Type a )
{
	if( !CheckArgumentLength( args, 5, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );

	// call the function
	method( arg1, arg2, arg3, arg4, arg5 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 5 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity5Type a )
{
	if( !CheckArgumentLength( args, 5, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );

	// call the function
	returnValueRef = method( arg1, arg2, arg3, arg4, arg5 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 6 member function
//////////////////////////////////////////////////////////////////////////
template<  typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity6Type a )
{
	if( !CheckArgumentLength( args, 6, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );

	// call the function
	method( arg1, arg2, arg3, arg4, arg5, arg6 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 6 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity6Type a )
{
	if( !CheckArgumentLength( args, 6, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );

	// call the function
	returnValueRef = method( arg1, arg2, arg3, arg4, arg5, arg6 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 7 member function
//////////////////////////////////////////////////////////////////////////
template<  typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity7Type a )
{
	if( !CheckArgumentLength( args, 7, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );
	EXTRACT_ARGUMENT_N( args, 7 );

	// call the function
	method( arg1, arg2, arg3, arg4, arg5, arg6, arg7 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 7 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity7Type a )
{
	if( !CheckArgumentLength( args, 7, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );
	EXTRACT_ARGUMENT_N( args, 7 );

	// call the function
	returnValueRef = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 8 member function
//////////////////////////////////////////////////////////////////////////
template<  typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity8Type a )
{
	if( !CheckArgumentLength( args, 8, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );
	EXTRACT_ARGUMENT_N( args, 7 );
	EXTRACT_ARGUMENT_N( args, 8 );

	// call the function
	method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 8 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity8Type a )
{
	if( !CheckArgumentLength( args, 8, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );
	EXTRACT_ARGUMENT_N( args, 7 );
	EXTRACT_ARGUMENT_N( args, 8 );

	// call the function
	returnValueRef = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// VOID Arity 9 member function
//////////////////////////////////////////////////////////////////////////
template<  typename fnType >
bool BlueCallFunction( fnType method, BlueScriptArguments args, arity9Type a )
{
	if( !CheckArgumentLength( args, 9, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );
	EXTRACT_ARGUMENT_N( args, 7 );
	EXTRACT_ARGUMENT_N( args, 8 );
	EXTRACT_ARGUMENT_N( args, 9 );

	// call the function
	method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 );
	return true;
};

//////////////////////////////////////////////////////////////////////////
// NON-VOID Arity 9 member function
//////////////////////////////////////////////////////////////////////////
template< typename fnType, typename R >
bool BlueCallFunction( fnType method, BlueScriptArguments args, R& returnValueRef, arity9Type a )
{
	if( !CheckArgumentLength( args, 9, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );
	EXTRACT_ARGUMENT_N( args, 6 );
	EXTRACT_ARGUMENT_N( args, 7 );
	EXTRACT_ARGUMENT_N( args, 8 );
	EXTRACT_ARGUMENT_N( args, 9 );

	// call the function
	returnValueRef = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 );
	return true;
};

#undef EXTRACT_ARGUMENT_N

//////////////////////////////////////////////////////////////////////////
// BlueCallFunctionReturningValue
//
// This branches on return type being Be::Result or something else
//

template< typename fnType >
BlueScriptValue BlueCallFunctionReturningValue( fnType method, BlueScriptArguments args, std::true_type isBeResult )
{
	typedef
		typename function_traits<fnType>::return_type
		return_type;

	return_type result;
	BlueScriptValue returnValue;
	if( !BlueCallFunctionWithBeResult( method, args, result, returnValue, typename function_traits<fnType>::arity_type() ) )
	{
		return BLUE_SCRIPT_ERROR; // Error set by BlueCallFunction
	}

	if( !BeIsSuccess( result ) )
	{
#if BLUE_WITH_PYTHON
		CCP_ASSERT( returnValue == nullptr );
		PyErr_SetString( BeGetException( result ), BeGetErrorMessage( result ) );
#endif
		return BLUE_SCRIPT_ERROR;
	}
	return returnValue;
}

template< typename fnType >
BlueScriptValue BlueCallFunctionReturningValue( fnType method, BlueScriptArguments args, std::false_type isBeResult )
{
	typedef
		typename function_traits<fnType>::return_type
		return_type;

	typedef
		typename remap_wchar_to_string<typename remove_const_and_reference<return_type>::type>::type
		stripped_return_type;

	stripped_return_type p;
	if( !BlueCallFunction( method, args, p, typename function_traits<fnType>::arity_type() ) )
	{
		return BLUE_SCRIPT_ERROR;   // Error set by BlueCallFunction
	}

	return BlueWrapReturnValueHelper( args, p, typename is_pointer_to_blue<stripped_return_type>::type() );
}

//////////////////////////////////////////////////////////////////////////
// BlueCallFunctionAndWrapReturn
//
// This template uses overloading on the last argument to resolve void 
// vs non-void returns

template< typename fnType >
BlueScriptValue BlueCallFunctionAndWrapReturn( fnType method, BlueScriptArguments args, std::true_type isVoid )
{
	if( !BlueCallFunction( method, args, typename function_traits<fnType>::arity_type() ) )
	{
		return BLUE_SCRIPT_ERROR;   // Error set by BlueCallFunction
	}

	BLUE_SCRIPT_NO_RETURN_VALUE;
}

template< typename fnType>
BlueScriptValue BlueCallFunctionAndWrapReturn( fnType method, BlueScriptArguments args, std::false_type isVoid )
{
	typedef
		typename function_traits<fnType>::return_type
		return_type;

	return BlueCallFunctionReturningValue( method, args, typename is_be_result<return_type>::type() );
}

#endif // BlueCallFunction_h