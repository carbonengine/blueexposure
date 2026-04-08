// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueCallFunctionWithBeResult_h
#define BlueCallFunctionWithBeResult_h

#include "BlueWrapReturnValue.h"

template< typename resultType, typename argType >
BlueScriptValue BlueGetReturnValueIfSuccess( BlueScriptArguments args, const resultType& result, const argType& arg )
{
	if( BeIsSuccess( result ) )
	{
		return BlueWrapReturnValue( args, arg );
	}
	else
	{
		return BLUE_SCRIPT_ERROR;
	}
}

template< typename resultType>
BlueScriptValue BlueGetVoidReturnValueIfSuccess( const resultType& result )
{
	if( BeIsSuccess( result ) )
	{
		BLUE_SCRIPT_NO_RETURN_VALUE;
	}
	else
	{
		return BLUE_SCRIPT_ERROR;
	}
}

template< typename resultType>
BlueScriptValue BlueGetWrappedObjectReturnValueIfSuccess( BlueScriptArguments args, const resultType& result, IRoot* p )
{
	if( BeIsSuccess( result ) )
	{
		if( p )
		{
			BlueScriptValue returnValue = BlueWrapReturnValue( args, p );
			p->Unlock();
			return returnValue;
		}
		else
		{
			BLUE_SCRIPT_NO_RETURN_VALUE;
		}
	}
	else
	{
		CCP_ASSERT( !p );
		return BLUE_SCRIPT_ERROR;
	}
}

//////////////////////////////////////////////////////////////////////////
// Arity 0 with Be::Result - returns None to Python
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity0Type a )
{
	if( !CheckArgumentLength( args, 0, 0 ) )
	{
		return false;
	}

	result = method();

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 1

// Arity 1 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity1Type a )
{
	typedef typename function_traits<fnType>::arg1_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( method, args, result, returnValue, a, typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method()
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity1Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg1_type A1;

	// The C++ function is arity 1, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 0, 0 ) )
	{
		return false;
	}

	typename std::remove_pointer<A1>::type p{nullptr};
	result = method( &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity1Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg1_type A1;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<A1>::type() );
}

// Be::Result classType::method( A1 a1 )
// In Python, this is called as
//  classType.method(a1)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity1Type a, std::false_type isLastArgRef )
{
	if( !CheckArgumentLength( args, 1, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );

	result = method( arg1 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1& a1 )
// In Python, this is called as
//  x = classType.method()
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity1Type a, std::true_type isLastArgRef )
{
	typedef typename function_traits<fnType>::arg1_type lastArgType;

	// The C++ function is arity 1, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 0, 0 ) )
	{
		return false;
	}

	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );
	return true;
}




//////////////////////////////////////////////////////////////////////////
// Arity 2

// Arity 2 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity2Type a )
{
	typedef typename function_traits<fnType>::arg2_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity2Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg2_type lastArgType;

	// The C++ function is arity 2, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 1, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity2Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg2_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2 )
// In Python, this is called as
//  classType.method(a1, a2)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity2Type a, std::false_type isLastArgRef )
{
	if( !CheckArgumentLength( args, 2, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );

	result = method( arg1, arg2 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2& a2 )
// In Python, this is called as
//  x = classType.method(a1)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity2Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 2, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 1, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );

	typedef typename function_traits<fnType>::arg2_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );

	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 3

// Arity 3 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity3Type a )
{
	typedef typename function_traits<fnType>::arg3_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, A2 arg2, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1, arg2)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity3Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg3_type lastArgType;

	// The C++ function is arity 3, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 2, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, arg2, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity3Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg3_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3 )
// In Python, this is called as
//  classType.method(a1, a2, a3)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity3Type a, std::false_type isLastArgRef )
{
	if( !CheckArgumentLength( args, 3, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );

	result = method( arg1, arg2, arg3 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2 a2, A3& a3 )
// In Python, this is called as
//  x = classType.method(a1, a2)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity3Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 3, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 2, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );

	typedef typename function_traits<fnType>::arg3_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, arg2, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 4

// Arity 4 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity4Type a )
{
	typedef typename function_traits<fnType>::arg4_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, A2 arg2, A3, arg3, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1, arg2, arg3)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity4Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg4_type lastArgType;

	// The C++ function is arity 4, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 3, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, arg2, arg3, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity4Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg4_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4 )
// In Python, this is called as
//  classType.method(a1, a2, a3, a4)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity4Type a, std::false_type isLastArgRef )
{
	if( !CheckArgumentLength( args, 4, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );

	result = method( arg1, arg2, arg3, arg4 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4& a4 )
// In Python, this is called as
//  x = classType.method(a1, a2, a3)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity4Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 4, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 3, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );

	typedef typename function_traits<fnType>::arg4_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, arg2, arg3, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );

	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 5

// Arity 5 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity5Type a )
{
	typedef typename function_traits<fnType>::arg5_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, A2 arg2, A3 arg3, A4 arg4, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1, arg2, arg3, arg4)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity5Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg5_type lastArgType;

	// The C++ function is arity 5, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 4, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, arg2, arg3, arg4, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity5Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg5_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
// In Python, this is called as
//  classType.method(a1, a2, a3, a4, a5)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity5Type a, std::false_type isLastArgRef )
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

	result = method( arg1, arg2, arg3, arg4, arg5 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5& a5 )
// In Python, this is called as
//  x = classType.method(a1, a2, a3, a4)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity5Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 5, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 4, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );

	typedef typename function_traits<fnType>::arg5_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, arg2, arg3, arg4, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );

	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 6

// Arity 6 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity6Type a )
{
	typedef typename function_traits<fnType>::arg6_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1, arg2, arg3, arg4, arg5)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity6Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg6_type lastArgType;

	// The C++ function is arity 6, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 5, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, arg2, arg3, arg4, arg5, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity6Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg6_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
// In Python, this is called as
//  classType.method(a1, a2, a3, a4, a5, a6)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity6Type a, std::false_type isLastArgRef )
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

	result = method( arg1, arg2, arg3, arg4, arg5, arg6 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6& a6 )
// In Python, this is called as
//  x = classType.method(a1, a2, a3, a4, a5)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity6Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 6, but the last argument is actually the return value
	if( !CheckArgumentLength( args, 5, function_traits<fnType>::numOptional ) )
	{
		return false;
	}

	EXTRACT_ARGUMENT_N( args, 1 );
	EXTRACT_ARGUMENT_N( args, 2 );
	EXTRACT_ARGUMENT_N( args, 3 );
	EXTRACT_ARGUMENT_N( args, 4 );
	EXTRACT_ARGUMENT_N( args, 5 );

	typedef typename function_traits<fnType>::arg6_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, arg2, arg3, arg4, arg5, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 7

// Arity 7 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity7Type a )
{
	typedef typename function_traits<fnType>::arg7_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1, arg2, arg3, arg4, arg5, arg6)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity7Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg7_type lastArgType;

	// The C++ function is arity 7, but the last argument is actually the return value
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

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, arg2, arg3, arg4, arg5, arg6, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity7Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg7_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
// In Python, this is called as
//  classType.method(a1, a2, a3, a4, a5, a6, a7)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity7Type a, std::false_type isLastArgRef )
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

	result = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7& a7 )
// In Python, this is called as
//  x = classType.method(a1, a2, a3, a4, a5, a6)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity7Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 7, but the last argument is actually the return value
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

	typedef typename function_traits<fnType>::arg7_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, arg2, arg3, arg4, arg5, arg6, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 8

// Arity 8 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity8Type a )
{
	typedef typename function_traits<fnType>::arg8_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1, arg2, arg3, arg4, arg5, arg6, arg7)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity8Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg8_type lastArgType;

	// The C++ function is arity 8, but the last argument is actually the return value
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

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity8Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg8_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
// In Python, this is called as
//  classType.method(a1, a2, a3, a4, a5, a6, a7, a8)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity8Type a, std::false_type isLastArgRef )
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

	result = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8& a8 )
// In Python, this is called as
//  x = classType.method(a1, a2, a3, a4, a5, a6, a7)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity8Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 8, but the last argument is actually the return value
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

	typedef typename function_traits<fnType>::arg8_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Arity 9

// Arity 9 with Be::Result - this function branches on whether the last argument is IRoot**
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResult( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity9Type a )
{
	typedef typename function_traits<fnType>::arg9_type lastArgType;

	return BlueCallFunctionWithBeResultReturningValue( 
		method, args, result, returnValue, a, 
		typename is_pointer_to_pointer_to_blue<lastArgType>::type() );
}

// Be::Result classType::method( A1 arg1, A2 arg2, A3 arg3, A4 arg4, A5 arg5, A6 arg6, A7 arg7, A7 arg8, <IRoot descendant>** returnValue )
// In Python, this is called as
//  x = classType.method(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity9Type a, std::true_type returnsObject )
{
	typedef typename function_traits<fnType>::arg9_type lastArgType;

	// The C++ function is arity 9, but the last argument is actually the return value
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

	typename std::remove_pointer<lastArgType>::type p{nullptr};
	result = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, &p );

	returnValue = BlueGetWrappedObjectReturnValueIfSuccess( args, result, reinterpret_cast<IRoot*>( p ) );
	return true;
}

template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultReturningValue( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity9Type a, std::false_type returnsObject )
{
	typedef typename function_traits<fnType>::arg9_type lastArgType;

	return BlueCallFunctionWithBeResultLastArgRef( method, args, result, returnValue, a, typename std::is_reference<lastArgType>::type() );
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
// In Python, this is called as
//  classType.method(a1, a2, a3, a4, a5, a6, a7, a8, a9)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity9Type a, std::false_type isLastArgRef )
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

	result = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9 );

	returnValue = BlueGetVoidReturnValueIfSuccess( result );
	return true;
}

// Be::Result classType::method( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9& a9 )
// In Python, this is called as
//  x = classType.method(a1, a2, a3, a4, a5, a6, a7, a8)
// and raises an exception if Be::Result is not a success code.
template< typename fnType, typename resultType >
bool BlueCallFunctionWithBeResultLastArgRef( fnType method, BlueScriptArguments args, resultType& result, BlueScriptValue& returnValue, arity9Type a, std::true_type isLastArgRef )
{
	// The C++ function is arity 9, but the last argument is actually the return value
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

	typedef typename function_traits<fnType>::arg9_type lastArgType;
	typename std::remove_reference<lastArgType>::type lastArg;

	result = method( arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, lastArg );

	returnValue = BlueGetReturnValueIfSuccess( args, result, lastArg );
	return true;
}


#endif // BlueCallFunctionWithBeResult_h
