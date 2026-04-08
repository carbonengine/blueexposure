// Copyright © 2026 CCP ehf.

BLUEIMPORT PyObject* PyThunkLeave(PyObject* ret);

template< class POINTER_TYPE > POINTER_TYPE BluePythonCast( PyObject* self );

template< typename FnType, FnType fn >
PyObject* BlueFunctionHelper( PyObject* self, PyObject* args )
{
	typedef typename function_traits<FnType>::return_type return_type;
	return BlueCallFunctionAndWrapReturn( fn, args, typename std::is_void<return_type>::type() );
}

typedef PyObject* (*PyMethodSignature)( PyObject* self, PyObject* args );

BLUEIMPORT PyObject* PyThunkLeave(PyObject* ret);

template<PyMethodSignature func>
static PyObject* BlueFunctionAsMethodHelper( PyObject* self, PyObject* args )
{
	return PyThunkLeave( (func)( self, args) );
}

template< typename memFnType, memFnType memFn >
PyObject* BlueMethodHelper( PyObject* self, PyObject* args )
{
	typedef typename function_traits<memFnType>::return_type return_type;
	typedef typename function_traits<memFnType>::class_type class_type;

	// Extract the class pointer
	class_type* pThis = BluePythonCast<class_type*>( self );

	BlueMemberFunctionAsCallable<memFnType> callable( pThis, memFn );

	// We always need to deal with void carefully.
	// so start by overloading specifically for that special case
	return BlueCallFunctionAndWrapReturn(
		callable,
		args,
		typename std::is_void<return_type>::type() );

}

template< typename memFnType, memFnType memFn, int numOptional >
PyObject* BlueMethodHelperWithOptionalArgs( PyObject* self, PyObject* args )
{
	typedef typename function_traits<memFnType>::return_type return_type;
	typedef typename function_traits<memFnType>::class_type class_type;

	// Extract the class pointer
	class_type* pThis = BluePythonCast<class_type*>( self );

	BlueFunctionWithOptionalArguments< BlueMemberFunctionAsCallable< memFnType >, numOptional > callable( pThis, memFn );

	// We always need to deal with void carefully.
	// so start by overloading specifically for that special case
	return BlueCallFunctionAndWrapReturn(
		callable,
		args,
		typename std::is_void<return_type>::type() );

}

template< typename memFnType, memFnType memFn >
PyObject* BlueMethodAsMethodHelper( PyObject* self, PyObject* args )
{
	typedef typename function_traits<memFnType>::class_type class_type;

	// Extract the class pointer
	class_type* pThis = BluePythonCast<class_type*>( self );

	PyObject *result =(pThis->*memFn)(args);
	if( !result )
	{
		result = PyThunkLeave(result);
	}
	return result;
}

template <typename memFnType, memFnType memFn>
PyObject* BlueMethodWithKeywordArgumentsHelper( PyObject* self, PyObject* args, PyObject* kwargs )
{
	typedef typename function_traits<memFnType>::class_type class_type;

	// Extract the class pointer
	class_type* pThis = BluePythonCast<class_type*>( self );

	PyObject* result = ( pThis->*memFn )( args, kwargs );
	if( !result )
	{
		result = PyThunkLeave( result );
	}
	return result;
}

// Wrapper for a property setter function
template< typename Class, typename setterFnType, setterFnType setterFn >
PyObject* BluePropertySetter( PyObject* self, PyObject* args )
{
	Class* pThis = BluePythonCast<Class*>( self );
	if( !BlueCallPropertySetter( setterFn, pThis, args, typename is_be_result<typename function_traits<setterFnType>::return_type>::type() ) )
	{
		return nullptr;
	}
	else
	{
		Py_RETURN_NONE;
	}
}

// Wrapper for a property getter function
template< typename Class, typename getterFnType, getterFnType getterFn >
PyObject* BluePropertyGetter( PyObject* self )
{
	Class* pThis = BluePythonCast<Class*>( self );
	return BlueCallPropertyGetter( getterFn, pThis, nullptr, typename is_be_result<typename function_traits<getterFnType>::return_type>::type() );
}
