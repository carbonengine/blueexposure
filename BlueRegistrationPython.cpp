// Copyright © 2026 CCP ehf.

#if BLUE_WITH_PYTHON

#include "include/BlueRegistration.h"

#include "BluePythonThunkers.h"

bool GetEnumValueName_Impl( const EnumValues &values, uint32_t val, std::string& out );
bool GetEnumValuesAsBitMask_Impl( const EnumValues &values, uint32_t val, std::string& out );

PyObject* PyExc_BlueError = nullptr;

namespace
{
	// This class has one pointer, to something that's effectively an everlasting
	// singleton. We don't need to delete the pointer when it dies.
	// It's a pure python class, in order to have more control over its behaviour
	class PyBlueEnumObject : public PyObject
	{
	public:
		const EnumValues* enumValues;
	};

	// Forward declarations for the methods struct definition
	static PyObject* PyBlueEnumObject_GetAttrString(PyObject  *o, char* attr_name);
	static PyObject* PyBlueEnumObject_Dir(PyObject *self, PyObject * args );
	static PyObject* PyGetNameFromBitmask( PyObject* self, PyObject* args );
	static PyObject* PyGetNameFromValue( PyObject* self, PyObject* args );

	// Required for Python objects
	static void	BlueEnumObject_dealloc(PyObject* self)
	{
		PyObject_Del(self);
	};
	
	// The python class functions on this enum type
	static PyMethodDef PyBlueEnumObjectType_methods[] = {
		{"__dir__", (PyCFunction)PyBlueEnumObject_Dir, METH_NOARGS,
		"Returns a list of the enumeration names for this enum"
		},
		{"GetNameFromValue", (PyCFunction)PyGetNameFromValue, METH_VARARGS,
		"Returns a string corresponding to the first value found that exactly matches the input\n" 
		":param value: enum value\n"
		":type value: int\n"
		":rtype: str"
		},
		{"GetNameFromBitmask", (PyCFunction)PyGetNameFromBitmask, METH_VARARGS,
		"Attempts to match a value by treating it as a bitmask\n"
		":param values: bitmask of enum values\n"
		":type values: int\n"
		":rtype: str"
		},
		{NULL}
	};

	// --------------------------------------------------------------------------------------
	// Description:
	//   GetAttr support for enumeration names as attributes on the class
	// --------------------------------------------------------------------------------------
	static PyObject* PyBlueEnumObject_GetAttrString(PyObject  *o, char* attr_name)
	{
		PyBlueEnumObject* e = static_cast<PyBlueEnumObject*>( o );
		if( !e->enumValues )
		{
			return NULL;
		}

		// We're either looking for the value of an enum on this object
		for( auto enumIt = e->enumValues->begin(); enumIt != e->enumValues->end(); ++enumIt )
		{
			if( strcmp(enumIt->mKey, attr_name) == 0 )
			{
				return PyInt_FromLong(enumIt->mValue.mLong);
			}
		}

		// Or we're looking for one of the utility methods
		PyObject* meth = Py_FindMethod( PyBlueEnumObjectType_methods, o, attr_name );
		if( meth )
		{
			return meth;
		}

		PyErr_SetString( PyExc_AttributeError, "Enum value / function does not exist" );
		return NULL;
	};

	// --------------------------------------------------------------------------------------
	// Description:
	//   Python method: This function explicitly overrides the normal attribute lookup methods
	//   it provides a way for us to dynamically generate attributes on this class that
	//   have the enum names and values
	// --------------------------------------------------------------------------------------
	static PyObject* PyBlueEnumObject_Dir(PyObject *self, PyObject * args )
	{
		PyBlueEnumObject* e = static_cast<PyBlueEnumObject*>( self );
		if( !e->enumValues )
		{
			return NULL;
		}

		// Expose the enum names as attributes
		PyObject* l = PyList_New( e->enumValues->size() );
		size_t i = 0;
		for( auto enumIt = e->enumValues->begin(); enumIt != e->enumValues->end(); ++enumIt, ++i )
		{
			PyList_SET_ITEM( l, i, PyString_FromString( enumIt->mKey ) );
		}

		// Expose the built in methods from the methods struct array
		PyMethodDef* iter = PyBlueEnumObjectType_methods;
		while( iter->ml_meth )
		{
			PyObject* o = PyString_FromString( iter->ml_name );
			PyList_Append( l, o );
			Py_DECREF( o );
			++iter;
		}

		// TODO: expose built in attributes like '__class__' etc

		return l;
	};

	// --------------------------------------------------------------------------------------
	// Description:
	//   Converts an enum value into an enum name, returning the first exact matching
	//   enumeration name that it finds. 
	// Arguments:
	//   enumValue - integer value
	// --------------------------------------------------------------------------------------
	static PyObject* PyGetNameFromValue( PyObject* self, PyObject* args )
	{
		PyBlueEnumObject* e = static_cast<PyBlueEnumObject*>( self );
		
		if( !e->enumValues )
		{
			return NULL;
		}

		int val;
		if( !PyArg_ParseTuple( args, "i", &val ) )
		{
			return NULL;
		}

		std::string result;
		bool found = GetEnumValueName_Impl( *(e->enumValues), (uint32_t)val, result );
		if( found )
		{
			return PyString_FromStringAndSize( result.c_str(), result.size());
		}
		else
		{
			PyErr_SetString( PyExc_AttributeError, "Enum value not found" );
			return NULL;
		}
	}

	// --------------------------------------------------------------------------------------
	// Description:
	//   Converts an bitmask value into an ' | ' separated string of enum names
	// Arguments:
	//   enumValue - integer value
	// --------------------------------------------------------------------------------------
	static PyObject* PyGetNameFromBitmask( PyObject* self, PyObject* args )
	{
		PyBlueEnumObject* e = static_cast<PyBlueEnumObject*>( self );

		if( !e->enumValues )
		{
			return NULL;
		}

		int val;
		if( !PyArg_ParseTuple( args, "i", &val ) )
		{
			return NULL;
		}

		std::string result;
		bool found = GetEnumValuesAsBitMask_Impl( *(e->enumValues), (uint32_t)val, result );
		if( found )
		{
			return PyString_FromStringAndSize( result.c_str(), result.size());
		}
		else
		{
			PyErr_SetString( PyExc_AttributeError, "Enum value not found" );
			return NULL;
		}
	}

	// --------------------------------------------------------------------------------------
	// Description:
	//   The standard python type object
	// --------------------------------------------------------------------------------------
	static PyTypeObject PyBlueEnumObjectType = {
		PyObject_HEAD_INIT(NULL)
		0,
		"blue.BlueEnum",
		sizeof(PyBlueEnumObject),
		0,
		BlueEnumObject_dealloc,			/*tp_dealloc*/
		0,								/*tp_print*/
		PyBlueEnumObject_GetAttrString, /*tp_getattr*/
		0,								/* tp_setattr        */
		0,								/* tp_compare        */
		0,								/* tp_repr           */
		0,								/* tp_as_number      */
		0,								/* tp_as_sequence    */
		0,								/* tp_as_mapping     */
		0,								/* tp_hash           */
		0,								/* tp_call           */
		0,								/* tp_str            */
		0,								/* tp_getattro       */
		0,								/* tp_setattro       */
		0,								/* tp_as_buffer      */
		Py_TPFLAGS_DEFAULT,				/* tp_flags          */
		0,								/* tp_doc            */
		0,								/* tp_traverse       */
		0,								/* tp_clear          */
		0,								/* tp_richcompare    */
		0,								/* tp_weaklistoffset */
		0,								/* tp_iter           */
		0,								/* tp_iternext       */
		PyBlueEnumObjectType_methods,	/* tp_methods        */
		0,								/* tp_members        */
		0,								/* tp_getset         */
		0,								/* tp_base           */
		0,								/* tp_dict           */
		0,								/* tp_descr_get      */
		0,								/* tp_descr_set      */
		0,								/* tp_dictoffset     */
		0,								/* tp_init           */
	};

}

// --------------------------------------------------------------------------------------
// Description:
//   Adds classes, module functions and enumerations to the module at initialization
// --------------------------------------------------------------------------------------

BLUEIMPORT void BlueRegisterToModule(
	PyObject* module, 
	const ClassRegsVector& classRegs, 
	const FuncRegsVector& funcRegs,						   
	const EnumRegsMap& enumRegs, 
	const TestRegsVector& testRegs,
	const ThunkerRegsVector& thunkerRegs,
	const FuncSignatures& funcSignatures
	)
{
	if( !PyExc_BlueError )
	{
		PyExc_BlueError = PyErr_NewException( const_cast<char*>("blue.error"), PyExc_StandardError, NULL);
	}

	PyObject* dict = PyModule_GetDict( module );

	//Dynamically adding module level functions
	//for( PyMethodDef* it = &funcRegs[0]; it->ml_name; ++it )
	for( auto it = funcRegs.begin(); it != funcRegs.end(); ++it )
	{
		const PyMethodDef& thunker = *it;
		PyObject* thunk = PyCFunction_New(const_cast<PyMethodDef*>( &thunker ), module);
		PyDict_SetItemString(dict, thunker.ml_name, thunk);
		Py_DECREF(thunk);
	}

	if( !classRegs.empty() )
	{
		// Add module thunkers
		BlueRegisterModuleThunkers( module, &classRegs[0], classRegs.size() );
	}


	for ( auto it = classRegs.begin(); it != classRegs.end(); ++it )
	{
		const Be::ClassRegistration& cr = *it;

		// The type objects were set up in the registration but finalizing
		// the type has to happen after Python is initialized so we do it
		// here.
		PyType_Ready( cr.mType->mTypeObject );
		Py_INCREF( cr.mType->mTypeObject );


		if( cr.mFlags & Be::ClassRegistration::DISABLE_PYTHON_CONSTRUCTION )
		{
			continue;
		}

		PyModule_AddObject( module, cr.mType->mClassId->GetName(), (PyObject*)cr.mType->mTypeObject );
	}

	// Make sure that the enum type is initialized
	PyType_Ready( &PyBlueEnumObjectType );
	
	// Dynamically add enums
	for( auto it = enumRegs.begin(); it != enumRegs.end(); ++it )
	{
		const EnumValues& vals = it->second.second();

		// In the normal case, register the enum values themselves into the module
		if( it->second.first & ENUM_REG_VALUES_ON_MODULE )
		{
			// Add the individual values in this enum
			for( auto enumIt = vals.begin(); enumIt != vals.end(); ++enumIt )
			{
				PyModule_AddIntConstant( module, enumIt->mKey, enumIt->mValue.mLong );
			}
		}
		// In this case, we register the enumeration itself to the module
		if( it->second.first & ENUM_REG_ENUM_OBJECT_ON_MODULE )
		{
			PyObject* o = PyObject_New( PyBlueEnumObject, &PyBlueEnumObjectType );
			PyBlueEnumObject* e = static_cast<PyBlueEnumObject*>( o );
			e->enumValues = &vals;
			PyModule_AddObject( module, it->first.c_str(), o );
		}
	}

	if( !testRegs.empty() )
	{
		PyObject* unittestmodule = PyModule_New("unittests");
		PyObject* unittestdict = PyModule_GetDict(unittestmodule);
		for( auto it = testRegs.begin(); it != testRegs.end(); ++it )
		{
			const PyMethodDef& thunker = *it;	
			PyObject* thunk = PyCFunction_New(const_cast<PyMethodDef*>( &thunker ), unittestmodule);
			PyDict_SetItemString(unittestdict, thunker.ml_name, thunk);
			Py_DECREF(thunk);
		}
		PyModule_AddObject( module, "unittests", unittestmodule);
	}

	if( !funcSignatures.empty() )
	{
		PyObject* signatures = PyDict_New();

		for( auto it = funcSignatures.begin(); it != funcSignatures.end(); ++it )
		{
			PyObject* signature = PyDict_New();
			PyObject* tmp;
			tmp = PyString_FromString( it->second.returnType );
			PyDict_SetItemString( signature, "rtype", tmp );
			Py_DECREF( tmp );
			PyObject* args = PyTuple_New( it->second.argumentCount );
			for( uint32_t j = 0; j < it->second.argumentCount; ++j )
			{
				PyTuple_SET_ITEM( args, j, PyString_FromString( it->second.argumentTypes[j] ) );
			}
			PyDict_SetItemString( signature, "parameters", args );
			Py_DECREF( args );

			PyObject* count = PyInt_FromLong( it->second.optionalCount );
			PyDict_SetItemString( signature, "optionalCount", count );
			Py_DECREF( count );

			PyDict_SetItemString( signatures, it->first.c_str(), signature );
			Py_DECREF( signature );
		}

		PyModule_AddObject( module, "__func_signatures__", signatures );
	}

	ThunkerRegsVector& globalRegs = BlueRegistration::GetGlobalThunkerRegs();
	for( ThunkerRegsVector::const_iterator it = thunkerRegs.begin(); it != thunkerRegs.end(); ++it )
	{
		globalRegs.push_back( *it );
	}
}

BLUEIMPORT void BlueRegisterObjectsToModule( PyObject* module, ObjectRegsVector& objectRegs )
{
	for( auto it = objectRegs.begin(); it != objectRegs.end(); ++it )
	{
		if( *it->second )
		{
			PyModule_AddObject( module, it->first, BlueWrapObjectForPython( *it->second ) );
		}
		else
		{
			CCP_LOGERR( "Attempting to register '%s' to module but objects is a nullptr", it->first );
		}
	}
}

BLUEIMPORT void BlueRegisterExceptionsToModule( PyObject* module, ExceptionRegsMap& exceptionRegs )
{
	for( auto it = exceptionRegs.begin(); it != exceptionRegs.end(); ++it )
	{
		auto exc = ( *it->second )();
		Py_INCREF( exc );
		PyModule_AddObject( module, it->first.c_str(), exc );
	}
}

#endif
