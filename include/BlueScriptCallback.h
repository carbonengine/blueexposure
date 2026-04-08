// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueScriptCallback_H
#define BlueScriptCallback_H

#include "BlueTypes.h"
#include "BlueExtractArgument.h"
#include "BlueWrapReturnValue.h"

// Status of call
// Note that clang doesn't allow BLUEIMPORT on an embedded class - have to
// have this toplevel rather than inside the scope of BlueScriptCallback.
class BLUEIMPORT BlueScriptCallbackStatus
{
public:
	BlueScriptCallbackStatus( const BlueScriptCallbackStatus& );
	BlueScriptCallbackStatus& operator=( const BlueScriptCallbackStatus& );
	~BlueScriptCallbackStatus();
	operator bool() const;
	void MuteException();
	void ReportException();
private:
	enum Status
	{
		OK,
		CALL_ERROR,
		EXCEPTION
	};
	BlueScriptCallbackStatus( Status hasException );

	Status m_hasException;
	bool m_muteException;
	bool m_exceptionReported;

#if BLUE_WITH_PYTHON
	PyObject* m_type;
	PyObject* m_value;
	PyObject* m_traceback;
#endif
	friend class BLUEIMPORT BlueScriptCallback;
};

// --------------------------------------------------------------------------------------
// Description:
//   BlueScriptCallback encapsulates script callback functions. One can store a script 
//   function in BlueScriptCallback and subsequently call it from C++.
// --------------------------------------------------------------------------------------
class BLUEIMPORT BlueScriptCallback
{
public:
	BlueScriptCallback();
	BlueScriptCallback( const BlueScriptCallback& other );
	~BlueScriptCallback();

	BlueScriptCallback& operator=( const BlueScriptCallback& other );

	bool IsValid() const;
	operator bool() const { return IsValid(); }

	void Destroy();

	BlueScriptCallbackStatus CallVoid();
	
	template <typename Ret>
	BlueScriptCallbackStatus Call( Ret& returnValue );

	template <typename A0>
	BlueScriptCallbackStatus CallVoid( A0 a0 );

	template <typename Ret, typename A0>
	BlueScriptCallbackStatus Call( Ret& returnValue, A0 a0 );

	template <typename A0, typename A1>
	BlueScriptCallbackStatus CallVoid( A0 a0, A1 a1 );

	template <typename Ret, typename A0, typename A1>
	BlueScriptCallbackStatus Call( Ret& returnValue, A0 a0, A1 a1 );

	template <typename A0, typename A1, typename A2>
	BlueScriptCallbackStatus CallVoid( A0 a0, A1 a1, A2 a2 );

	template <typename Ret, typename A0, typename A1, typename A2>
	BlueScriptCallbackStatus Call( Ret& returnValue, A0 a0, A1 a1, A2 a2 );

	template <typename A0, typename A1, typename A2, typename A3>
	BlueScriptCallbackStatus CallVoid( A0 a0, A1 a1, A2 a2, A3 a3 );

	template <typename Ret, typename A0, typename A1, typename A2, typename A3>
	BlueScriptCallbackStatus Call( Ret& returnValue, A0 a0, A1 a1, A2 a2, A3 a3 );

	template <typename A0, typename A1, typename A2, typename A3, typename A4>
	BlueScriptCallbackStatus CallVoid( A0 a0, A1 a1, A2 a2, A3 a3, A4 a4 );

	template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
	BlueScriptCallbackStatus CallVoid( A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5);

	template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
	BlueScriptCallbackStatus CallVoid( A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6);

private:
	BlueScriptValue m_callback;

	friend BLUEIMPORT BlueScriptValue BlueWrapReturnValueImpl( 
		BlueScriptArguments args, 
		const BlueScriptCallback& val );
	friend BLUEIMPORT bool BlueExtractArgumentImpl( 
		BlueScriptValue argument, 
		BlueScriptCallback& result, 
		unsigned int argID, 
		std::false_type isBlueType );

	template <typename T>
	static inline void RefIRoot( T obj, std::false_type isBlueObj )
	{
	}

	template <typename T>
	static inline void RefIRoot( T obj, std::true_type isBlueObj )
	{
		obj->Lock();
	}

	template <typename T>
	static inline void RefIRoot( T obj )
	{
		BlueScriptCallback::RefIRoot( obj, typename is_pointer_to_blue<T>::type() );
	}

};

inline void BlueGetNullValue( BlueScriptCallback& resultRef )
{
	resultRef = BlueScriptCallback();
}


template <typename Ret>
BlueScriptCallbackStatus BlueScriptCallback::Call( Ret& returnValue )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, nullptr );
	if( ret )
	{
		if( !BlueExtractArgument( ret, returnValue, 0 ) )
		{
			return BlueScriptCallbackStatus::EXCEPTION;
		}
		BlueScriptCallback::RefIRoot( returnValue );
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename A0>
BlueScriptCallbackStatus BlueScriptCallback::CallVoid( A0 a0 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, nullptr );
	Py_DECREF( arg0 );
	if( ret )
	{
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename Ret, typename A0>
BlueScriptCallbackStatus BlueScriptCallback::Call( Ret& returnValue, A0 a0 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, nullptr );
	Py_DECREF( arg0 );
	if( ret )
	{
		if( !BlueExtractArgument( ret, returnValue, 0 ) )
		{
			return BlueScriptCallbackStatus::EXCEPTION;
		}
		BlueScriptCallback::RefIRoot( returnValue );
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename A0, typename A1>
BlueScriptCallbackStatus BlueScriptCallback::CallVoid( A0 a0, A1 a1 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	if( ret )
	{
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename Ret, typename A0, typename A1>
BlueScriptCallbackStatus BlueScriptCallback::Call( Ret& returnValue, A0 a0, A1 a1 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	if( ret )
	{
		if( !BlueExtractArgument( ret, returnValue, 0 ) )
		{
			return BlueScriptCallbackStatus::EXCEPTION;
		}
		BlueScriptCallback::RefIRoot( returnValue );
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename A0, typename A1, typename A2>
BlueScriptCallbackStatus BlueScriptCallback::CallVoid( A0 a0, A1 a1, A2 a2 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	BlueScriptValue arg2 = BlueWrapReturnValue( args, a2 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, arg2, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	Py_DECREF( arg2 );
	if( ret )
	{
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename Ret, typename A0, typename A1, typename A2>
BlueScriptCallbackStatus BlueScriptCallback::Call( Ret& returnValue, A0 a0, A1 a1, A2 a2 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	BlueScriptValue arg2 = BlueWrapReturnValue( args, a2 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, arg2, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	Py_DECREF( arg2 );
	if( ret )
	{
		if( !BlueExtractArgument( ret, returnValue, 0 ) )
		{
			return BlueScriptCallbackStatus::EXCEPTION;
		}
		BlueScriptCallback::RefIRoot( returnValue );
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename A0, typename A1, typename A2, typename A3>
BlueScriptCallbackStatus BlueScriptCallback::CallVoid( A0 a0, A1 a1, A2 a2, A3 a3 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	BlueScriptValue arg2 = BlueWrapReturnValue( args, a2 );
	BlueScriptValue arg3 = BlueWrapReturnValue( args, a3 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, arg2, arg3, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	Py_DECREF( arg2 );
	Py_DECREF( arg3 );
	if( ret )
	{
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename Ret, typename A0, typename A1, typename A2, typename A3>
BlueScriptCallbackStatus BlueScriptCallback::Call( Ret& returnValue, A0 a0, A1 a1, A2 a2, A3 a3 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	BlueScriptValue arg2 = BlueWrapReturnValue( args, a2 );
	BlueScriptValue arg3 = BlueWrapReturnValue( args, a3 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, arg2, arg3, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	Py_DECREF( arg2 );
	Py_DECREF( arg3 );
	if( ret )
	{
		if( !BlueExtractArgument( ret, returnValue, 0 ) )
		{
			return BlueScriptCallbackStatus::EXCEPTION;
		}
		BlueScriptCallback::RefIRoot( returnValue );
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}


template <typename A0, typename A1, typename A2, typename A3, typename A4>
BlueScriptCallbackStatus BlueScriptCallback::CallVoid( A0 a0, A1 a1, A2 a2, A3 a3, A4 a4 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	BlueScriptValue arg2 = BlueWrapReturnValue( args, a2 );
	BlueScriptValue arg3 = BlueWrapReturnValue( args, a3 );
	BlueScriptValue arg4 = BlueWrapReturnValue( args, a4 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, arg2, arg3, arg4, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	Py_DECREF( arg2 );
	Py_DECREF( arg3 );
	Py_DECREF( arg4 );
	if( ret )
	{
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
BlueScriptCallbackStatus BlueScriptCallback::CallVoid( A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	BlueScriptValue arg2 = BlueWrapReturnValue( args, a2 );
	BlueScriptValue arg3 = BlueWrapReturnValue( args, a3 );
	BlueScriptValue arg4 = BlueWrapReturnValue( args, a4 );
	BlueScriptValue arg5 = BlueWrapReturnValue( args, a5 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, arg2, arg3, arg4, arg5, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	Py_DECREF( arg2 );
	Py_DECREF( arg3 );
	Py_DECREF( arg4 );
	Py_DECREF( arg5 );
	if( ret )
	{
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}

template <typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
BlueScriptCallbackStatus BlueScriptCallback::CallVoid( A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
	if( !IsValid() )
	{
		return BlueScriptCallbackStatus::CALL_ERROR;
	}

#if BLUE_WITH_PYTHON
	auto gil = PyGILState_Ensure();
	ON_BLOCK_EXIT( [&gil] { PyGILState_Release( gil ); } );

	BlueScriptArguments args = 0;

	BlueScriptValue arg0 = BlueWrapReturnValue( args, a0 );
	BlueScriptValue arg1 = BlueWrapReturnValue( args, a1 );
	BlueScriptValue arg2 = BlueWrapReturnValue( args, a2 );
	BlueScriptValue arg3 = BlueWrapReturnValue( args, a3 );
	BlueScriptValue arg4 = BlueWrapReturnValue( args, a4 );
	BlueScriptValue arg5 = BlueWrapReturnValue( args, a5 );
	BlueScriptValue arg6 = BlueWrapReturnValue( args, a6 );
	PyObject* ret = PyObject_CallFunctionObjArgs( m_callback, arg0, arg1, arg2, arg3, arg4, arg5, arg6, nullptr );
	Py_DECREF( arg0 );
	Py_DECREF( arg1 );
	Py_DECREF( arg2 );
	Py_DECREF( arg3 );
	Py_DECREF( arg4 );
	Py_DECREF( arg5 );
	Py_DECREF( arg6 );
	if( ret )
	{
		Py_DECREF( ret );
		return BlueScriptCallbackStatus::OK;
	}
	return BlueScriptCallbackStatus::EXCEPTION;
#elif BLUE_NO_EXPOSURE
	return BlueScriptCallbackStatus::EXCEPTION;
#endif
}

#endif