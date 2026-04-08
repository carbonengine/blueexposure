// Copyright © 2026 CCP ehf.

#include "BlueVariable.h"

static bool CompareStrings(const char* a, const char* b)
{
	if( a == b )
	{
		return true;
	}
	else if( !a || !b )
	{
		return false;
	}
	else
	{
		return strcmp( a, b ) == 0;
	}
}

static bool CompareWStrings(const wchar_t* a, const wchar_t* b)
{
	if( a == b )
	{
		return true;
	}
	else if( !a || !b )
	{
		return false;
	}
	else
	{
		return wcscmp(a, b) == 0;
	}
}

#define FLOAT_PRECISION_ERROR 1e-6f

static bool IsArrayEqual( const float* a, const float* b, size_t numElements )
{
	for( size_t i = 0; i < numElements; ++i )
	{
		float d = a[i] - b[i];
		if( fabs( d ) >= FLOAT_PRECISION_ERROR )
		{
			return false;
		}
	}
	return true;
}

#if BLUE_WITH_PYTHON
PyObject* PyFloatTupleOfTuples_FromFloatMatrix( const float* values, size_t valuesCount )
{
	int dimension = 0;
	if( valuesCount == 16 )
	{
		dimension = 4;
	}
	else if( valuesCount == 9 )
	{
		dimension = 3;
	}
	// Note: We currently don't handle other dimensions.  Can be added here if the need
	// arises.  It would also be wasteful to generalize this support by calling SQRT.
	// It is conceivable that we want to support 4x3 matrices since those are quite useful
	// and SQRT wouldn't work for those.  So stop the speculative generality and move on :)
	// <halldor 2008-11-13>

	PyObject* ret = PyTuple_New( dimension );

	const float* array = values;
	for( int i = 0; i < dimension; ++i )
	{
		PyObject* tuple = PyTuple_New( dimension );
		// Marshall row 'i' in the array as a 'dimension'-tuple
		for( int k = 0; k < dimension; ++k )
		{
			PyTuple_SET_ITEM( tuple, k, PyFloat_FromDouble( array[dimension*i + k ] ) );
		}
		// Collect the tuple into a tuple of rows
		PyTuple_SET_ITEM( ret, i, tuple );
	}

	return ret;
}

PyObject* PyFloatTuple_FromFloatVector( const float* values, size_t valuesCount )
{
	PyObject* r = PyTuple_New( valuesCount );
	for( size_t i = 0; i < valuesCount; ++i )
	{
		PyTuple_SET_ITEM( r, i, PyFloat_FromDouble( values[i] ) );
	}
	return r;
}

PyObject* PyObject_FromFloatArray( const float* values, size_t valuesCount, const Be::IID* iid )
{
	if( iid == &BlueMatrixIID )
	{
		return PyFloatTupleOfTuples_FromFloatMatrix( values, valuesCount );
	}
	else
	{
		return PyFloatTuple_FromFloatVector( values, valuesCount );
	}
}
#endif

template <Be::VARTYPE V> bool AreEqual(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return false;
}

#if BLUE_WITH_PYTHON
template <Be::VARTYPE V> bool ConvertFromPython(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	CCP_LOGERR( "Error converting type from blue from python.");
	return false;
}

template <Be::VARTYPE V> void ConvertToPython(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	CCP_LOGERR( "Error converting type from blue to python." );
	PyErr_SetString(PyExc_TypeError, "Error converting type from blue to python.");
}
#endif

template <Be::VARTYPE V> bool Copy(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	CCP_LOGERR( "Cannot copy variable type" );
	return false;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::INVALID>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	return false;
}

template<> void ConvertToPython<Be::INVALID>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = Py_None;
	Py_INCREF(ret);
}
#endif

template<> bool AreEqual<Be::LONG>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return a->mLong == b->mLong;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::LONG>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	return BlueExtractInt( v, value->mLong );
}

template<> void ConvertToPython<Be::LONG>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyInt_FromLong(value->mLong);
}
#endif

template<> bool Copy<Be::LONG>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mLong = src->mLong;
	return true;
}


template<> bool AreEqual<Be::FLOAT>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	float d = a->mFloat - b->mFloat;
	return fabs( d ) < FLOAT_PRECISION_ERROR;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::FLOAT>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	return BlueExtractFloat( v, value->mFloat );
}

template<> void ConvertToPython<Be::FLOAT>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyFloat_FromDouble(value->mFloat);
}
#endif

template<> bool Copy<Be::FLOAT>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mFloat = src->mFloat;
	return true;
}


template<> bool AreEqual<Be::DOUBLE>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	double d = a->mDouble - b->mDouble;
	return fabs( d ) < 1e-12;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::DOUBLE>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	return 	BlueExtractDouble( v, value->mDouble );
}

template<> void ConvertToPython<Be::DOUBLE>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyFloat_FromDouble(value->mDouble);
}
#endif

template<> bool Copy<Be::DOUBLE>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mDouble = src->mDouble;
	return true;
}


template<> bool AreEqual<Be::BOOL>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return a->mBool == b->mBool;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::BOOL>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	return BlueExtractBool( v, value->mBool );
}

template<> void ConvertToPython<Be::BOOL>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyBool_FromLong(value->mBool);
}
#endif

template<> bool Copy<Be::BOOL>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mBool = src->mBool;
	return true;
}


template<> bool AreEqual<Be::IROOT>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	
	// We're not really handling the generic case of embedded objects
	// but we are checking the common case of empty lists.
	{
		IListPtr listA( BlueCastPtr( reinterpret_cast<IRoot*>( const_cast<Be::Var*>( a ) ) ) );
		IListPtr listB( BlueCastPtr( reinterpret_cast<IRoot*>( const_cast<Be::Var*>( b ) ) ) );
		if( listA && listB )
		{
			if( (listA->GetSize() == 0) && (listB->GetSize() == 0) )
			{
				return true;
			}
		}
	}

	{
		IBlueDictPtr dictA( BlueCastPtr( reinterpret_cast<IRoot*>( const_cast<Be::Var*>( a ) ) ) );
		IBlueDictPtr dictB( BlueCastPtr( reinterpret_cast<IRoot*>( const_cast<Be::Var*>( b ) ) ) );
		if( dictA && dictB )
		{
			if( (dictA->GetLength() == 0) && (dictB->GetLength() == 0) )
			{
				return true;
			}
		}
	}

	{
		IBlueStructureListPtr listA( BlueCastPtr( reinterpret_cast<IRoot*>( const_cast<Be::Var*>( a ) ) ) );
		IBlueStructureListPtr listB( BlueCastPtr( reinterpret_cast<IRoot*>( const_cast<Be::Var*>( b ) ) ) );
		if( listA && listB )
		{
			if( (listA->GetSize() == 0) && (listB->GetSize() == 0) )
			{
				return true;
			}
		}
	}
	
	return false;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::IROOT>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	IRoot* obj = BlueUnwrapObjectFromPython(v);
		
	if (!obj)
	{
		PyErr_SetString(PyExc_TypeError, "Value ain't IRoot object");
		return false;
	}

	CCopier copier;
	return copier.CopyTo(obj, (IRoot**)&value);
}

template<> void ConvertToPython<Be::IROOT>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = BlueWrapObjectForPython( const_cast<IRoot*>( reinterpret_cast<const IRoot*>( value ) ) );
}
#endif

template<> bool Copy<Be::IROOT>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	if ( !copier->CopyTo( reinterpret_cast<IRoot*>( src ), reinterpret_cast<IRoot**>( &dst ) ) )
					return false;
	return true;
}


template<> bool AreEqual<Be::IROOTPTR>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return a->mIRootPtr == b->mIRootPtr;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::IROOTPTR>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if (v == Py_None)
	{
		// just kill it
		if (value->mIRootPtr)
		{
			value->mIRootPtr->Unlock();
			value->mIRootPtr = NULL;
		}
	}
	else
	{
		IRoot* obj = BlueUnwrapObjectFromPython(v);
			
		if (!obj)
		{
			PyErr_SetString(PyExc_TypeError, "Value ain't IRoot object");
			return false;
		}

		if (var->mIID)
		{
			// typed assignment
			IRoot* tmp = nullptr;
			obj->QueryInterface(*var->mIID, (void**)&tmp, BEQI_NONE);

			if (!tmp)
			{
				PyErr_SetString(PyExc_TypeError, "value lacks the correct interface");
				return false;
			}

			if (value->mIRootPtr)
				value->mIRootPtr->Unlock();
				
			value->mIRootPtr = tmp;
		}
		else
		{
			if (value->mIRootPtr)
				value->mIRootPtr->Unlock();

			value->mIRootPtr = obj;
			value->mIRootPtr->Lock();
		}				
	}
		
	return true;
}

template<> void ConvertToPython<Be::IROOTPTR>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	if (!value->mIRootPtr)
	{
		ret = Py_None;
		Py_INCREF(ret);
	}
	else
	{
		ret = BlueWrapObjectForPython(value->mIRootPtr);
	}
}
#endif

template<> bool Copy<Be::IROOTPTR>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	// Release any destination object after we finish
	IRootPtr dstOld;
	dstOld.Attach( dst->mIRootPtr ); // Don't INCREF what was there
	// unlock the old destination object when we go out of scope
	// NB: This should prevent things from blowing up when you reference the same IRootPtr multiple times in the same class
	// which can be useful behavior for dealing with legacy code
	dst->mIRootPtr = NULL;

	if (!src->mIRootPtr)
		return false;
	IRootPtr tmp;
	if (!copier->CopyTo(src->mIRootPtr, &tmp))
		return false;
	//Query the correct interface
	static Be::IID irootType( "IRoot" );
	const Be::IID &iid = entry->mIID ? *entry->mIID : irootType;
	if (!tmp->QueryInterface(iid, (void**)&dst->mIRootPtr))
		return false;
	return true;
}


template<> bool AreEqual<Be::CHARARRAY>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return CompareStrings((const char*)a, (const char*)b);
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::CHARARRAY>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if (PyString_Check(v) == true)
	{
		if (PyString_GET_SIZE(v) >= (Py_ssize_t)var->mSize)
			PyErr_SetString(PyExc_OverflowError, "String is too large");
		else
			strcpy_s((char*)value, var->mSize, PyString_AS_STRING(v));
		return true;
	}
	return false;
}

template<> void ConvertToPython<Be::CHARARRAY>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyString_FromString((const char*)value);
}
#endif

template<> bool Copy<Be::CHARARRAY>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	memcpy(dst, src, entry->mSize);
	return true;
}


template<> bool AreEqual<Be::CSTRING>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return CompareStrings(a->mCharPtr, b->mCharPtr);
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::CSTRING>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if (!PyString_Check(v))
	{
		return false;
	}
	CCP_FREE(value->mWCharPtr);
	value->mCharPtr = CCP_STRDUP( __FUNCTION__, PyString_AS_STRING(v) );
	return true;
}

template<> void ConvertToPython<Be::CSTRING>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyString_FromString(value->mCharPtr ? value->mCharPtr : "");
}
#endif

template<> bool Copy<Be::CSTRING>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	CCP_FREE(dst->mCharPtr);
	dst->mCharPtr = CCP_STRDUP( __FUNCTION__, src->mCharPtr);
	return true;
}


template<> bool AreEqual<Be::INT64>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return a->mInt64 == b->mInt64;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::INT64>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	int64_t t;
	if (PyLong_Check(v))
	{
		t = PyLong_AsLongLong(v);
	} 
	else if (PyInt_Check(v))
	{
        t = PyInt_AsLong(v);
	}
    else
    {
        return false;
    }
	if (t == -1 && PyErr_Occurred())
    {
		return false;
    }
	value->mInt64 = t;
	return true;
}

template<> void ConvertToPython<Be::INT64>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyLong_FromLongLong(value->mInt64);
}
#endif

template<> bool Copy<Be::INT64>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mInt64 = src->mInt64;
	return true;
}

#if BLUE_WITH_PYTHON

template<> bool ConvertFromPython<Be::PYOBJECT>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	Py_XDECREF((PyObject*)value);
	value = (Be::Var*)v;
	Py_INCREF(v);
	return true;
}

template<> void ConvertToPython<Be::PYOBJECT>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = (PyObject*)value;
	Py_INCREF(ret);
}


template<> bool AreEqual<Be::PYOBJECT>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return a->mPyObject == b->mPyObject;
}

template<> bool ConvertFromPython<Be::PYOBJECTPTR>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	PyObject *tmp = value->mPyObject;
	value->mPyObject=v;
	Py_INCREF(v);
	Py_XDECREF(tmp);
	return true;
}

template<> void ConvertToPython<Be::PYOBJECTPTR>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = value->mPyObject ? value->mPyObject : Py_None;
	Py_INCREF(ret);
}

template<> bool Copy<Be::PYOBJECTPTR>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mPyObject = src->mPyObject;
	Py_XINCREF(dst->mPyObject);
	return true;
}

#endif


template<> bool AreEqual<Be::REFERENCE>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return CompareStrings(a->mCharPtr, b->mCharPtr);
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::REFERENCE>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if (!PyString_Check(v))
	{
		return false;
	}
	CCP_FREE(value->mWCharPtr);
	value->mCharPtr = CCP_STRDUP( __FUNCTION__, PyString_AS_STRING(v) );
	return true;
}

template<> void ConvertToPython<Be::REFERENCE>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const wchar_t *s = value->mWCharPtr ? value->mWCharPtr : L"";
	ret = PyUnicode_FromWideChar(s, wcslen(s));
}

template<> bool Copy<Be::REFERENCE>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	CCP_FREE(dst->mCharPtr);
	dst->mCharPtr = CCP_STRDUP( __FUNCTION__, src->mCharPtr);
	return true;
}
#endif


template<> bool AreEqual<Be::WCSTRING>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return CompareWStrings(a->mWCharPtr, b->mWCharPtr);
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::WCSTRING>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	PyObject *tmp = PyUnicode_FromObject(v);
	if(!tmp)
	{
		return false;
	}
	CCP_FREE(value->mWCharPtr);
	value->mWCharPtr = CCP_WSTRDUP( __FUNCTION__, (const wchar_t*)PyUnicode_AS_UNICODE(tmp) );
	Py_DECREF(tmp);
	return true;
}

template<> void ConvertToPython<Be::WCSTRING>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const wchar_t *s = value->mWCharPtr ? value->mWCharPtr : L"";
	ret = PyUnicode_FromWideChar(s, wcslen(s));
}
#endif

template<> bool Copy<Be::WCSTRING>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	CCP_FREE(dst->mWCharPtr);
	dst->mWCharPtr = CCP_WSTRDUP( __FUNCTION__, src->mWCharPtr);
	return true;
}


template<> bool AreEqual<Be::WREFERENCE>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return CompareWStrings(a->mWCharPtr, b->mWCharPtr);
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::WREFERENCE>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	PyObject *tmp = PyUnicode_FromObject(v);
	if (!tmp) 
	{ 
		return false;
	}
	CCP_FREE(value->mWCharPtr);
	value->mWCharPtr = CCP_WSTRDUP( __FUNCTION__, (const wchar_t*)PyUnicode_AS_UNICODE(tmp) );
	Py_DECREF(tmp);
	return true;
}

template<> void ConvertToPython<Be::WREFERENCE>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const wchar_t *s = value->mWCharPtr ? value->mWCharPtr : L"";
	ret = PyUnicode_FromWideChar(s, wcslen(s));
}
#endif

template<> bool Copy<Be::WREFERENCE>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	CCP_FREE(dst->mWCharPtr);
	dst->mWCharPtr = CCP_WSTRDUP( __FUNCTION__, src->mWCharPtr);
	return true;
}


template<> bool AreEqual<Be::FLOATARRAY>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return IsArrayEqual( &a->mFloat, &b->mFloat, entry->GetFloatArraySize() );
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::FLOATARRAY>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if( var->mIID == &BlueMatrixIID )
	{
		
		if( !BlueExtractMatrix( v, &value->mFloat, var->GetFloatArraySize() ) )
		{
			PyErr_Format( PyExc_TypeError, "%s can only be assigned to with a sequence of float sequences", 
							var->mName );
			return false;
		}
	}
	else
	{
		if( !BlueExtractVector( v, &value->mFloat, var->GetFloatArraySize() ) )
		{
			PyErr_Format( PyExc_TypeError, "%s can only be assigned to with a sequence of maximum %zu numbers", 
							var->mName, var->GetFloatArraySize() );
			return false;
		}

	}
	return true;
}

template<> void ConvertToPython<Be::FLOATARRAY>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyObject_FromFloatArray( (float*)&value->mFloat, entry->GetFloatArraySize(), entry->mIID );
}
#endif

template<> bool Copy<Be::FLOATARRAY>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	memcpy( (void*)&dst->mFloat, (void*)&src->mFloat, entry->mSize);
	return true;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::DOUBLEARRAY>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if( BlueExtractVector<double>( v, &value->mDouble, var->GetDoubleArraySize() ) )
	{
		return true;
	}
	PyErr_Format( PyExc_TypeError, "%s can only be assigned to with a sequence of maximum %zu numbers", 
					var->mName, var->GetDoubleArraySize() );
	return false;
}
#endif

template<> bool Copy<Be::DOUBLEARRAY>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	memcpy( (void*)&dst->mDouble, (void*)&src->mDouble, entry->mSize);
	return true;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::INTARRAY>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if( BlueExtractVector<int>( v, &value->mLong, var->GetIntArraySize() ) )
	{
		return true;
	}
	PyErr_Format( PyExc_TypeError, "%s can only be assigned to with a sequence of maximum %zu numbers", 
						var->mName, var->GetIntArraySize() );
	return false;
}
#endif

template<> bool Copy<Be::INTARRAY>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	memcpy( (void*)&dst->mLong, (void*)&src->mLong, entry->mSize);
	return true;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::IROOTWEAKREF>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	BlueWeakRefBase* weakRef = reinterpret_cast<BlueWeakRefBase*>( value );

	if (v == Py_None)
	{
		*weakRef = nullptr;
	}
	else
	{
		IRoot* obj = BlueUnwrapObjectFromPython(v);

		if (!obj)
		{
			PyErr_SetString(PyExc_TypeError, "Value is not an IRoot object");
			return false;
		}

		if (var->mIID)
		{
			// typed assignment
			IRootPtr tmp;
			obj->QueryInterface(*var->mIID, (void**)&tmp, BEQI_NONE);

			if (!tmp)
			{
				PyErr_SetString(PyExc_TypeError, "value lacks the correct interface");
				return false;
			}
		}

		*weakRef = obj;
	}
	return true;
}

template<> void ConvertToPython<Be::IROOTWEAKREF>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const BlueWeakRefBase* weakRef = reinterpret_cast<const BlueWeakRefBase*>( value );
	IRoot* obj = *weakRef;
	if( obj )
	{
		ret = BlueWrapObjectForPython( obj );
	}
	else
	{
		ret = Py_None;
		Py_INCREF(ret);
	}
}
#endif

template<> bool Copy<Be::IROOTWEAKREF>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	BlueWeakRefBase* srcWeakRef = reinterpret_cast<BlueWeakRefBase*>( src );
	BlueWeakRefBase* dstWeakRef = reinterpret_cast<BlueWeakRefBase*>( dst );

	*dstWeakRef = *srcWeakRef;
	return true;
}


template<> bool AreEqual<Be::STDSTRING>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	const std::string &aStr = *reinterpret_cast<const std::string*>(a);
	const std::string &bStr = *reinterpret_cast<const std::string*>(b);
	return aStr == bStr;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::STDSTRING>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	std::string& s = *reinterpret_cast<std::string*>(value);
	return BlueExtractString( v, s );
}

template<> void ConvertToPython<Be::STDSTRING>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const std::string &s = *reinterpret_cast<const std::string*>(value);
	ret = PyString_FromStringAndSize( s.c_str(), s.size() );
}
#endif

template<> bool Copy<Be::STDSTRING>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	const std::string &srcString = *reinterpret_cast<const std::string*>(src);
	std::string &dstString = *reinterpret_cast<std::string*>(dst);
	dstString = srcString;
	return true;
}


template<> bool AreEqual<Be::STDWSTRING>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	const std::wstring &aStr = *reinterpret_cast<const std::wstring*>(a);
	const std::wstring &bStr = *reinterpret_cast<const std::wstring*>(b);
	return aStr == bStr;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::STDWSTRING>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	std::wstring& s = *reinterpret_cast<std::wstring*>(value);
	return BlueExtractWString( v, s );
}

template<> void ConvertToPython<Be::STDWSTRING>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const std::wstring &s = *reinterpret_cast<const std::wstring*>(value);
	ret = PyUnicode_FromUnicode( (Py_UNICODE*)s.c_str(), s.size() );
}
#endif

template<> bool Copy<Be::STDWSTRING>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	const std::wstring &srcString = *reinterpret_cast<const std::wstring*>(src);
	std::wstring &dstString = *reinterpret_cast<std::wstring*>(dst);
	dstString = srcString;
	return true;
}


template<> bool AreEqual<Be::BYTE>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return a->mByte == b->mByte;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::BYTE>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if (!PyInt_Check(v))
	{
		return false;
	}
	value->mByte = (char)PyInt_AS_LONG(v);
	return true;
}

template<> void ConvertToPython<Be::BYTE>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyInt_FromLong(value->mByte);
}
#endif

template<> bool Copy<Be::BYTE>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mByte = src->mByte;
	return true;
}


template<> bool AreEqual<Be::SHORT>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	return a->mShort == b->mShort;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::SHORT>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	if (!PyInt_Check(v))
	{
		return false;
	}
	value->mShort = (short)PyInt_AS_LONG(v);
	return true;
}

template<> void ConvertToPython<Be::SHORT>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	ret = PyInt_FromLong(value->mShort);
}
#endif

template<> bool Copy<Be::SHORT>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	dst->mShort = src->mShort;
	return true;
}


#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::SCRIPTCALLBACK>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	BlueScriptCallback& s = *reinterpret_cast<BlueScriptCallback*>(value);
	return BlueExtractArgumentImpl( v, s, 0, std::false_type() );
}

template<> void ConvertToPython<Be::SCRIPTCALLBACK>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const BlueScriptCallback& s = *reinterpret_cast<const BlueScriptCallback*>( value );
	ret = BlueWrapReturnValueImpl( BlueScriptArguments(), s );
}
#endif


template<> bool AreEqual<Be::SHAREDSTRING>(const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b)
{
	const BlueSharedString &aStr = *reinterpret_cast<const BlueSharedString*>(a);
	const BlueSharedString &bStr = *reinterpret_cast<const BlueSharedString*>(b);
	return aStr == bStr;
}

#if BLUE_WITH_PYTHON
template<> bool ConvertFromPython<Be::SHAREDSTRING>(const Be::VarEntry* var, Be::Var* value, PyObject* v)
{
	BlueSharedString& s = *reinterpret_cast<BlueSharedString*>(value);
	std::string str;
	bool ok = BlueExtractString( v, str );
	s = BlueSharedString( str );
	return ok;
}

template<> void ConvertToPython<Be::SHAREDSTRING>(const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret)
{
	const BlueSharedString &s = *reinterpret_cast<const BlueSharedString*>( value );
	ret = PyString_FromString( s.c_str() );
}
#endif

template<> bool Copy<Be::SHAREDSTRING>(const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier)
{
	const BlueSharedString &srcString = *reinterpret_cast<const BlueSharedString*>(src);
	BlueSharedString &dstString = *reinterpret_cast<BlueSharedString*>(dst);
	dstString = srcString;
	return true;
}



template <>
bool AreEqual<Be::SHAREDSTRINGW>( const Be::VarEntry* entry, const Be::Var* a, const Be::Var* b )
{
	const BlueSharedStringW& aStr = *reinterpret_cast<const BlueSharedStringW*>( a );
	const BlueSharedStringW& bStr = *reinterpret_cast<const BlueSharedStringW*>( b );
	return aStr == bStr;
}

#if BLUE_WITH_PYTHON
template <>
bool ConvertFromPython<Be::SHAREDSTRINGW>( const Be::VarEntry* var, Be::Var* value, PyObject* v )
{
	BlueSharedStringW& s = *reinterpret_cast<BlueSharedStringW*>( value );
	std::wstring str;
	bool ok = BlueExtractWString( v, str );
	s = BlueSharedStringW( str );
	return ok;
}

template <>
void ConvertToPython<Be::SHAREDSTRINGW>( const Be::VarEntry* entry, const Be::Var* value, PyObject*& ret )
{
	const BlueSharedStringW& s = *reinterpret_cast<const BlueSharedStringW*>( value );
	ret = PyUnicode_FromUnicode( (Py_UNICODE*)s.c_str(), wcslen( s.c_str() ) );
}
#endif

template <>
bool Copy<Be::SHAREDSTRINGW>( const Be::VarEntry* entry, Be::Var* dst, Be::Var* src, Copier* copier )
{
	const BlueSharedStringW& srcString = *reinterpret_cast<const BlueSharedStringW*>( src );
	BlueSharedStringW& dstString = *reinterpret_cast<BlueSharedStringW*>( dst );
	dstString = srcString;
	return true;
}

static BlueVariable* VAR_TO_BLUE_VARIABLE[Be::VARTYPE_MAX + 1] = {NULL};



#if BLUE_WITH_PYTHON
#define INIT_BLUE_VAR(v) { \
	BlueVariable* w = new BlueVariable(); \
	w->AreEqual = AreEqual<v>; \
	w->ConvertFromPython = ConvertFromPython<v>; \
	w->ConvertToPython = ConvertToPython<v>; \
	w->Copy = Copy<v>; \
	VAR_TO_BLUE_VARIABLE[v] = w; \
}
#else
    #define INIT_BLUE_VAR(v) { \
    BlueVariable* w = new BlueVariable(); \
    w->AreEqual = AreEqual<v>; \
    w->Copy = Copy<v>; \
    VAR_TO_BLUE_VARIABLE[v] = w; \
}
#endif

void InitializeBlueVariables()
{
	INIT_BLUE_VAR(Be::INVALID)
	INIT_BLUE_VAR(Be::LONG)
	INIT_BLUE_VAR(Be::FLOAT)
	INIT_BLUE_VAR(Be::DOUBLE)
	INIT_BLUE_VAR(Be::BOOL)
	INIT_BLUE_VAR(Be::IROOT)
	INIT_BLUE_VAR(Be::IROOTPTR)
	INIT_BLUE_VAR(Be::CHARARRAY)
	INIT_BLUE_VAR(Be::CSTRING)
	INIT_BLUE_VAR(Be::INT64)
	INIT_BLUE_VAR(Be::PYOBJECT)
	INIT_BLUE_VAR(Be::PYOBJECTPTR)
	INIT_BLUE_VAR(Be::REFERENCE)
	INIT_BLUE_VAR(Be::WCSTRING)
	INIT_BLUE_VAR(Be::WREFERENCE)
	INIT_BLUE_VAR(Be::FLOATARRAY)
	INIT_BLUE_VAR(Be::DOUBLEARRAY)
	INIT_BLUE_VAR(Be::INTARRAY)
	INIT_BLUE_VAR(Be::ROTREFERENCE)
	INIT_BLUE_VAR(Be::IROOTWEAKREF)
	INIT_BLUE_VAR(Be::_PYPROPERTY)
	INIT_BLUE_VAR(Be::STDSTRING)
	INIT_BLUE_VAR(Be::STDWSTRING)
	INIT_BLUE_VAR(Be::BINARYBLOCK)
	INIT_BLUE_VAR(Be::BYTE)
	INIT_BLUE_VAR(Be::SHORT)
	INIT_BLUE_VAR(Be::SCRIPTCALLBACK)
	INIT_BLUE_VAR(Be::SHAREDSTRING)
	INIT_BLUE_VAR(Be::SHAREDSTRINGW)
}

static bool initialized = false;

BlueVariable* GetBlueVariableFromVarType( Be::VARTYPE type )
{
	if( !initialized )
	{
		InitializeBlueVariables();
		initialized = true;
	}
	return VAR_TO_BLUE_VARIABLE[type];
}