////////////////////////////////////////////////////////////////////////////////
//
// Creator:		Snorri Sturluson
// Created:		January 2013
// Copyright (c) 2026 CCP Games
//

#define NOMINMAX

#include "include/BlueExposureMacros.h"
#include "include/BlueVectorTypes.h"
#include <StringConversions.h>
#include <limits>

#if BLUE_WITH_PYTHON
static_assert( sizeof( wchar_t ) >= sizeof( Py_UNICODE ), "Must be able to cast Py_UNICODE to wchar_t" );

bool BlueExtractString( PyObject* obj, std::string& val )
{
	bool ok = false;

	Py_ssize_t length = 0;

	if ((ok = PyUnicode_Check(obj)))
	{
		const char* buffer = PyUnicode_AsUTF8AndSize( obj, &length );
		val.assign( buffer, length );
		ok = true;
	}

	return ok;
}

bool BlueExtractWString( PyObject* obj, std::wstring& val )
{
	if( !PyUnicode_Check( obj ) )
	{
		return false;
	}

	Py_ssize_t length = 0;
	wchar_t* buffer = PyUnicode_AsWideCharString( obj, &length );
	if( !buffer )
	{
		return false;
	}

	val.assign( buffer, length );
	PyMem_Free( buffer );

	return true;
}

bool BlueExtractBool( PyObject* obj, bool& value )
{
	if( PyLong_Check( obj ) )
	{
		value = PyLong_AS_LONG( obj ) ? true : false;
	}
	else
	{
		return false;
	}

	return true;
}

bool BlueExtractInt( PyObject* obj, int32_t& value )
{
	if( PyLong_Check( obj ) )
	{
		long longValue = PyLong_AsLong( obj );

		if( longValue == -1 && PyErr_Occurred() )
		{
			return false;
		}

		if( longValue > std::numeric_limits<int32_t>::max() )
		{
			PyErr_SetString( PyExc_OverflowError, "int too big to convert" );
			return false;
		}
		else if( longValue < std::numeric_limits<int32_t>::min() )
		{
			PyErr_SetString( PyExc_OverflowError, "int too small to convert" );
			return false;
		}

		value = (int32_t)longValue;
	}
	else
	{
		return false;
	}

	return true;
}

bool BlueExtractUInt( PyObject* obj, uint32_t& value )
{
	if( PyLong_Check( obj ) )
	{
		unsigned long longValue = PyLong_AsUnsignedLong( obj );

		if( longValue == -1 && PyErr_Occurred() )
		{
			return false;
		}

		if( longValue > std::numeric_limits<uint32_t>::max() )
		{
			PyErr_SetString( PyExc_OverflowError, "int too small to convert" );
			return false;
		}

		value = (uint32_t)longValue;
	}
	else
	{
		return false;
	}

	return true;
}

bool BlueExtractDouble( PyObject* obj, double& value )
{
	if( PyFloat_Check( obj ) )
	{
		value = PyFloat_AS_DOUBLE( obj );
	}
	else if( PyLong_CheckExact( obj ) )
	{
		value = (double)PyLong_AS_LONG( obj );
	}
	else
	{
		return false;
	}

	return true;
}

bool BlueExtractFloat( PyObject* obj, float& value )
{
	if( PyFloat_Check( obj ) )
	{
		value = (float)PyFloat_AS_DOUBLE( obj );
	}
	else if( PyLong_CheckExact( obj ) )
	{
		value = (float)PyLong_AS_LONG( obj );
	}
	else
	{
		return false;
	}

	return true;
}

bool BlueExtractItem( PyObject* obj, float& value )
{
	return BlueExtractFloat( obj, value );
}

bool BlueExtractItem( PyObject* obj, double& value )
{
	return BlueExtractDouble( obj, value );
}

bool BlueExtractItem( PyObject* obj, int& value )
{
	return BlueExtractInt( obj, value );
}

template<class T> bool BlueExtractVector( PyObject* sequence, T* elements, size_t elementsCount )
{
	if( !PySequence_Check( sequence ) )
	{
		return false;
	}

	Py_ssize_t tupleCount = PySequence_Size( sequence );
	if( tupleCount < 0 || (size_t)tupleCount > elementsCount )
	{
		return false;
	}

	for( Py_ssize_t i = 0; i < tupleCount; ++i )
	{
		PyObject* item = PySequence_ITEM( sequence, i );
		bool success = BlueExtractItem( item, elements[ i ] );
		Py_DECREF( item );

		if( !success )
		{
			return false;
		}
	}

	return true;
}
// Explicit instantiations
template bool BlueExtractVector( BlueScriptValue obj,    int* elements, size_t elementsCount );
template bool BlueExtractVector( BlueScriptValue obj,  float* elements, size_t elementsCount );
template bool BlueExtractVector( BlueScriptValue obj, double* elements, size_t elementsCount );


bool BlueExtractVector( PyObject* sequence, float* elements, size_t elementsCount )
{
	return BlueExtractVector<float>( sequence, elements, elementsCount );
}


bool BlueExtractMatrix( PyObject* sequence, float* elements, size_t elementsCount )
{
	size_t dimension = 0;
	if( elementsCount == 16 )
	{
		dimension = 4;
	}
	else if( elementsCount == 9 )
	{
		dimension = 3;
	}

	if( !PySequence_Check( sequence ) )
	{
		return false;
	}

	Py_ssize_t tupleCount = PySequence_Size( sequence );
	if( tupleCount != (Py_ssize_t)dimension )
	{
		return false;
	}

	float* offset = elements;
	for( ssize_t i = 0; i < tupleCount; ++i )
	{
		PyObject* subSequence = PySequence_ITEM( sequence, i );
		bool ok = BlueExtractVector( subSequence, offset, dimension );
		Py_DECREF( subSequence );
		if( !ok )
		{
			return false;
		}
		offset += dimension;
	}

	return true;
}


// Overload for Float argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, float& result, unsigned int argID, std::false_type isBlueType )
{
	bool success = BlueExtractFloat( argument, result );
	if( !success )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "float" );
		return false;
	}

	return true;
}

// Overload for Double argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, double& result, unsigned int argID, std::false_type isBlueType )
{
	bool success = BlueExtractDouble( argument, result );
	if( !success )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "double" );
		return false;
	}

	return true;
}

// Overload for PyObject* argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, PyObject*& result, unsigned int argID, std::false_type isBlueType )
{
	result = argument;
	return true;
}

// Overload for Int argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, int32_t& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check( argument ) )
	{
		long longResult = PyLong_AsLong( argument );

		if( longResult == -1 && PyErr_Occurred() )
		{
			return false;
		}

		// Check for overflow
		if( longResult > std::numeric_limits<int32_t>::max() )
		{
			PyErr_SetString( PyExc_OverflowError, "int too big to convert" );
			return false;
		}

		if( longResult < std::numeric_limits<int32_t>::min() )
		{
			PyErr_SetString( PyExc_OverflowError, "int too small to convert" );
			return false;
		}

		result = (int32_t)longResult;
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "int" );
		return false;
	}

	return true;
}

bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, uint32_t& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check( argument ) )
	{
		unsigned long longResult = PyLong_AsUnsignedLong( argument );

		if( longResult == -1 && PyErr_Occurred() )
		{
			return false;
		}

		// Check for overflow
		if( longResult > std::numeric_limits<uint32_t>::max() )
		{
			PyErr_SetString( PyExc_OverflowError, "int too big to convert" );
			return false;
		}

		if( longResult < std::numeric_limits<uint32_t>::min() )
		{
			PyErr_SetString( PyExc_OverflowError, "int too small to convert" );
			return false;
		}

		result = (uint32_t)longResult;
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "unsigned int" );
		return false;
	}

	return true;
}

bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, uint8_t& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check(argument) )
	{
		uint32_t tmpResult = uint32_t( PyLong_AsLong( argument ) );
		if( tmpResult > std::numeric_limits<uint8_t>::max() )
		{
			PyErr_Format( PyExc_OverflowError, argumentTypeMismatchString, argID, "uint8_t" );
			return false;
		}
		result = (uint8_t)tmpResult;
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "uint8_t" );
		return false;
	}

	return true;
}

bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, uint16_t& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check(argument) )
	{
		uint32_t tmpResult = PyLong_AsUnsignedLong( argument );
		if( tmpResult > std::numeric_limits<uint16_t>::max() )
		{
			PyErr_Format( PyExc_OverflowError, argumentTypeMismatchString, argID, "uint16_t" );
			return false;
		}
		result = (uint8_t)tmpResult;
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "uint16_t" );
		return false;
	}

	return true;
}

// Overload for boolean argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, bool& result, unsigned int argID, std::false_type isBlueType )
{
	bool success = BlueExtractBool( argument, result );
	if( !success )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "bool" );
		return false;
	}

	return true;
}

// Overload for signed int64 argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, int64_t& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check(argument) )
	{
		result = PyLong_AsLongLong( argument );
		if( result == -1 && PyErr_Occurred() )
		{
			return false;
		}
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "int64_t" );
		return false;
	}

	return true;

}


// Overload for unsigned int64 argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, uint64_t& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check(argument) )
	{
		result = PyLong_AsUnsignedLongLong( argument );
		if( result == -1 && PyErr_Occurred() )
		{
			return false;
		}
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "uint64_t" );
		return false;
	}
    
	return true;
    
}

#ifdef __APPLE__
// Clang on the Mac treats size_t as something different from uint32_t/uint64_t, whereas
// other compilers complain about redefinition of this function if we keep it in.
// Overload for unsigned size_t argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, size_t& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check(argument) )
	{
		result = PyLong_AsSize_t( argument );
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "size_t" );
		return false;
	}
    
	return true;
    
}
#endif

#ifdef _MSC_VER
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, unsigned long& result, unsigned int argID, std::false_type isBlueType )
{
	if( PyLong_Check(argument) )
	{
		result = (unsigned long)PyLong_AsUnsignedLong( argument );
		if ( (unsigned long)-1 && PyErr_Occurred() ) {
			return false;
		}
	}
	else
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "unsigned long" );
		return false;
	}

	return true;
}
#elif defined(__APPLE__)
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, long& result, unsigned int argID, std::false_type isBlueType )
{
    if( PyLong_Check(argument) )
    {
        result = PyLong_AsLong( argument );
    }
    else
    {
        PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "long" );
        return false;
    }
    
    return true;
}

#endif

// Overload for string argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, std::string& result, unsigned int argID, std::false_type isBlueType )
{
	bool success = BlueExtractString( argument, result );
	if( !success )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "string" );
		return false;
	}

	return true;
}

bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, const char*& result, unsigned int argID, std::false_type isBlueType )
{
	if( !PyUnicode_Check(argument) )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "string" );
		return false;
	}

	result = PyUnicode_AsUTF8AndSize( argument, nullptr );
	return true;
}

bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, std::wstring& result, unsigned int argID, std::false_type isBlueType )
{
	bool success = BlueExtractWString( argument, result );
	if( !success )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "wstring" );
		return false;
	}

	return true;
}

bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, const wchar_t*& result, unsigned int argID, std::false_type isBlueType )
{
	if( !PyUnicode_Check(argument) )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "wstring" );
		return false;
	}

	wchar_t *tmpWideChar = PyUnicode_AsWideCharString(argument, nullptr);
	result = CCP_WSTRDUP( __FUNCTION__, tmpWideChar );
	PyMem_Free(tmpWideChar);
	return true;
}

// Overload for Matrix argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Matrix& result, unsigned int argID, std::false_type isBlueType )
{
	if( !BlueExtractMatrix( argument, &result._11, sizeof(result)/sizeof(float) ) )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, "((float, float, float, float),(float, float, float, float),(float, float, float, float),(float, float, float, float))" );
		return false;
	}

	return true;
}

template<class T> bool ConvertSequenceToArray( PyObject* argument, T* targetElements, size_t targetElementsCount, unsigned int argID )
{
	bool success = BlueExtractVector<T>( argument, targetElements, targetElementsCount );
	if( !success )
	{
		PyErr_Format( PyExc_TypeError, 
			"Could not match argument %i to expected type: sequence of max %zu numbers expected.", 
			argID, targetElementsCount );
	}

	return success;
}

// Overload for Vector2 argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Vector2& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<float>( argument, (float*)&result, sizeof(result)/sizeof(float), argID );
}

// Overload for Vector2d argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Vector2d& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<double>( argument, (double*)&result, sizeof(result)/sizeof(double), argID );
}

// Overload for Vector3 argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Vector3& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<float>( argument, (float*)&result, sizeof(result)/sizeof(float), argID );
}

// Overload for Vector3d argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Vector3d& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<double>( argument, (double*)&result, sizeof(result)/sizeof(double), argID );
}

// Overload for Vector3i argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Vector3i& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<int>( argument, (int*)&result, sizeof(result)/sizeof(int), argID );
}

// Overload for Vector4 argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Vector4& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<float>( argument, (float*)&result, sizeof(result)/sizeof(float), argID );
}

// Overload for Vector4d argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Vector4d& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<double>( argument, (double*)&result, sizeof(result)/sizeof(double), argID );
}

// Overload for Color argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Color& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<float>( argument, (float*)&result, sizeof(result)/sizeof(float), argID );
}

// Overload for Quaternion argument extraction
bool BLUEIMPORT BlueExtractArgumentImpl( PyObject* argument, Quaternion& result, unsigned int argID, std::false_type isBlueType )
{
	return ConvertSequenceToArray<float>( argument, (float*)&result, sizeof(result)/sizeof(float), argID );
}

PyObject* ConvertMatrixToSequence( const Matrix* m )
{
	PyObject* ret = PyTuple_New( 4 );

	// Start of floating point of array so we can iterate simply over the data
	const float* array = &m->_11;
	for( int i = 0; i < 4; ++i )
	{
		PyObject* tuple = PyTuple_New( 4 );
		// Marshall row 'i' in the array as a 4-tuple
		for( int k = 0; k < 4; ++k )
		{
			PyTuple_SET_ITEM( tuple, k, PyFloat_FromDouble( array[4*i + k ] ) );
		}
		// Collect the tuple into a tuple of rows
		PyTuple_SET_ITEM( ret, i, tuple );
	}

	return ret;
}

#elif BLUE_NO_EXPOSURE

bool BlueExtractString( BlueScriptValue, std::string& )
{
	return false;
}

#endif
