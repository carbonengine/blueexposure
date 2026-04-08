// Copyright © 2026 CCP ehf.

#include "BluePythonObject.h"

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, BlueScriptValue val )
{
	Py_XINCREF( val );
	return val;
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, IRoot* val )
{
	if( val )
	{
		// We know that p is IRoot or derived class so we can simply cast to IRoot by
		// interpreting p as pointing to IRoot (this is safe because IRoot is always the
		// first inherited class in any branch of the inheritance graph!)
		return BlueWrapObjectForPython( val );
	}
	else
	{
		Py_RETURN_NONE;
	}
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, bool val )
{
	if( val )
	{
		Py_RETURN_TRUE;
	}
	else
	{
		Py_RETURN_FALSE;
	}
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, int val )
{
	return PyInt_FromLong( val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, uint32_t val )
{
	return PyInt_FromLong( val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, uint64_t val )
{
	return PyLong_FromUnsignedLongLong( val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, int64_t val )
{
	return PyLong_FromLongLong( val );
}

#ifdef __APPLE__
// Clang on the Mac treats size_t as something different from uint32_t/uint64_t, whereas
// other compilers complain about redefinition of this function if we keep it in.
inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, size_t val )
{
	return PyLong_FromSize_t( val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, long val )
{
	return PyLong_FromLong( val );
}
#endif

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, float val )
{
	return PyFloat_FromDouble( val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, double val )
{
	return PyFloat_FromDouble( val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Vector2& val )
{
	float* array = (float*)&val;
	return Py_BuildValue( "(ff)", array[0], array[1] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Vector2d& val )
{
	double* array = (double*)&val;
	return Py_BuildValue( "(dd)", array[0], array[1] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Vector3& val )
{
	float* array = (float*)&val;
	return Py_BuildValue( "(fff)", array[0], array[1], array[2] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Vector3d& val )
{
	double* array = (double*)&val;
	return Py_BuildValue( "(ddd)", array[0], array[1], array[2] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Vector3i& val )
{
	int* array = (int*)&val;
	return Py_BuildValue( "(iii)", array[0], array[1], array[2] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Vector4& val )
{
	float* array = (float*)&val;
	return Py_BuildValue( "(ffff)", array[0], array[1], array[2], array[3] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Vector4d& val )
{
	double* array = (double*)&val;
	return Py_BuildValue( "(dddd)", array[0], array[1], array[2], array[3] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Color& val )
{
	float* array = (float*)&val;
	return Py_BuildValue( "(ffff)", array[0], array[1], array[2], array[3] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Quaternion& val )
{
	float* array = (float*)&val;
	return Py_BuildValue( "(ffff)", array[0], array[1], array[2], array[3] );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const Matrix& val )
{
	return ConvertMatrixToSequence( &val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const std::string& val )
{
	return PyString_FromStringAndSize( val.c_str(), val.size() );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const char* val )
{
	return PyString_FromString( val );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const std::wstring& val )
{
	return PyUnicode_FromWideChar( val.c_str(), val.size() );
}

inline BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const wchar_t* val )
{
	return PyUnicode_FromWideChar( val, wcslen( val ) );
}

template< typename T >
BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const std::vector<T>& val )
{
	PyObject* returnValue = PyList_New( val.size() );

	for( size_t i = 0; i < val.size(); ++i )
	{
		const T& item = val[i];

		PyObject* pyItem = BlueWrapReturnValue( args, item );
		PyList_SET_ITEM( returnValue, i, pyItem );
	}

	return returnValue;
}

template< typename T >
BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const std::list<T>& val )
{
	PyObject* returnValue = PyList_New( val.size() );

	size_t i = 0;
	for( auto it = val.begin(); it != val.end(); ++it, ++i )
	{
		const T& item = *it;

		PyObject* pyItem = BlueWrapReturnValue( args, item );
		PyList_SET_ITEM( returnValue, i, pyItem );
	}

	return returnValue;
}

template< typename T1, typename T2 >
BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const std::pair<T1, T2>& val )
{
	PyObject* returnValue = PyTuple_New( 2 );

	PyObject* pyFirst = BlueWrapReturnValue( args, val.first );
	PyObject* pySecond = BlueWrapReturnValue( args, val.second );

	PyTuple_SET_ITEM( returnValue, 0, pyFirst );
	PyTuple_SET_ITEM( returnValue, 1, pySecond );

	return returnValue;
}

template< typename KeyType, typename ValueType >
BlueScriptValue BlueWrapReturnValueImpl( BlueScriptArguments args, const std::map<KeyType, ValueType>& val )
{
	PyObject* returnValue = PyDict_New();

	for( auto it = val.begin(); it != val.end(); ++it )
	{
		PyObject* pyKey = BlueWrapReturnValue( args, it->first );
		PyObject* pyValue = BlueWrapReturnValue( args, it->second );

		PyDict_SetItem( returnValue, pyKey, pyValue );

		Py_XDECREF( pyKey );
		Py_XDECREF( pyValue );
	}

	return returnValue;
}


template< typename valueType>
BlueScriptValue BlueWrapReturnValueHelper( BlueScriptArguments args, valueType val, std::true_type isPointerToBlue )
{
	if( val )
	{
		// We know that p is IRoot or derived class so we can simply cast to IRoot by
		// interpreting p as pointing to IRoot (this is safe because IRoot is always the
		// first inherited class in any branch of the inheritance graph!)
		return BlueWrapObjectForPython( reinterpret_cast<IRoot*>( val ) );
	}
	else
	{
		Py_RETURN_NONE;
	}
}

