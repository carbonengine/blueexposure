// Copyright © 2026 CCP ehf.

static const char* const argumentTypeMismatchString = "Could not match argument %i to expected type: %s.";

template< class POINTER_TYPE > POINTER_TYPE BluePythonCast( PyObject* self );

// Template for IRoot* types
template< typename T >
bool BlueExtractArgumentImpl( BlueScriptValue argument, T& result, unsigned int argID,  std::true_type isBlueType )
{
	T pObject = BluePythonCast<T>( argument );

	// None is allowed as a NULL value, which will be the result of BluePythonCast on None anyway
	if( !pObject && argument != Py_None )
	{
		PyErr_Format( PyExc_TypeError, argumentTypeMismatchString, argID, typeid(typename std::remove_pointer<T>::type).name() );
		return false;
	}

	result = pObject;
	return true;
}

bool BLUEIMPORT BlueExtractArgumentImpl( BlueScriptValue argument, PyObject*& result, unsigned int argID, std::false_type isBlueType );

template< typename T>
bool BlueExtractArgumentImpl( BlueScriptValue argument, std::vector<T>& result, unsigned int argID, std::false_type isBlueType )
{
	if( !PySequence_Check( argument ) )
	{
		PyErr_Format( PyExc_TypeError, 
			"Could not match argument %i to expected type: sequence expected.", argID );
		return false;
	}

	Py_ssize_t n = PySequence_Size( argument );
	for( Py_ssize_t i = 0; i < n; ++i )
	{
		T item;
		PyObject* arg = PySequence_GetItem( argument, i );
		bool itemResult = BlueExtractArgument( arg, item, 0 );
		Py_XDECREF( arg );
		if( !itemResult )
		{
			PyErr_Format( PyExc_TypeError, 
				"Could not match item %zi in argument %u to expected type.", i, argID );
			return false;
		}

		result.push_back( item );
	}

	return true;
}

template< typename T1, typename T2 >
bool BlueExtractArgumentImpl( BlueScriptValue argument, std::pair<T1, T2>& result, unsigned int argID, std::false_type isBlueType )
{
	if( !PyTuple_Check( argument ) )
	{
		PyErr_Format( PyExc_TypeError, 
			"Could not match argument %i to expected type: tuple expected.", argID );
		return false;
	}

	Py_ssize_t n = PyTuple_Size( argument );
	if( n != 2 )
	{
		PyErr_Format( PyExc_TypeError, 
			"Could not match argument %i to expected type: tuple with two elements expected.", argID );
		return false;
	}

	T1 first;
	BlueExtractArgument( PyTuple_GET_ITEM( argument, 0 ), first, 0 );
	T2 second;
	BlueExtractArgument( PyTuple_GET_ITEM( argument, 1 ), second, 1 );

	result = std::make_pair( first, second );
	return true;
}

template< typename KeyType, typename ValueType>
bool BlueExtractArgumentImpl( BlueScriptValue argument, std::map<KeyType, ValueType>& result, unsigned int argID, std::false_type isBlueType )
{
	if( !PyDict_Check( argument ) )
	{
		PyErr_Format( PyExc_TypeError, 
			"Could not match argument %i to expected type: dict expected.", argID );
		return false;
	}

	PyObject* keyObj = nullptr;
	PyObject* valueObj = nullptr;
	Py_ssize_t pos = 0;

	while( PyDict_Next( argument, &pos, &keyObj, &valueObj ) )
	{
		KeyType key;
		bool keyResult = BlueExtractArgument( keyObj, key, 0 );
		if( !keyResult )
		{
			PyErr_Format( PyExc_TypeError, "Found a key of an incorrect type" );
			return false;
		}

		ValueType value;
		bool valueResult = BlueExtractArgument( valueObj, value, 0 );
		if( !valueResult )
		{
			PyErr_Format( PyExc_TypeError, "Found a value of an incorrect type" );
			return false;
		}

		result[key] = value;
	}

	return true;
}

