// Copyright © 2026 CCP ehf.

// Call this macro in a public section of your class declaration, then implement one static method in a cpp file:
//
// const Be::ClassInfo* <ClassName>::ExposeToBlue()
#define EXPOSE_TO_BLUE() \
	static const Be::ClassInfo* ClassType_() { return ExposeToBlue(); } \
	static const Be::Clsid* Clsid() { return ClassType_()->mClassId; } \
	static const Be::IID* IID() { return ClassType_()->mIID; } \
	\
	IRoot* GetRawRoot() const { return reinterpret_cast<IRoot*>((uintptr_t)this + ClassType_()->mInterfaceTable->mOffset); } \
	\
	const Be::ClassInfo* ClassType() const override { return ClassType_(); } \
	\
	static PyObject* PyNew( PyTypeObject* type, PyObject* args, PyObject* kwds ) \
	{\
		return BlueCreateInstanceFromPython( *Clsid(), args, kwds );\
	}\
	\
	static const Be::ClassInfo* ExposeToBlue()


#define EXPOSURE_BEGIN_IMP(_classname, _doc, _classid)\
	static Be::ClassInfo* s_classInfo = nullptr; \
	if( s_classInfo ) { return s_classInfo; } \
	typedef _classname _Class;\
	\
	const char* const _tmpdoc = _doc;\
	const Be::Clsid& _tmpclsid = _classid;\
	static Be::IID s_iid(#_classname); \
	static Be::IID s_iroot("IRoot"); \
	\
	static PyTypeObject s_pyType = { PyObject_HEAD_INIT(&PyType_Type) }; \
	static std::vector<PyMethodDef> s_methods;\
	static std::vector<Be::VarEntry> s_attributes; \
	static std::vector<Be::InterfaceEntry> s_interfaces; \
	static std::map<std::string, Be::BlueExposureFunctionSignature> s_signatures; \
	Be::InterfaceEntry rootEntry = {&s_iroot, BLUE_INTERFACEOFFSET(_Class)}; \
	s_interfaces.push_back( rootEntry ); \
	Be::InterfaceEntry myEntry = {&s_iid, BLUE_INTERFACEOFFSET(_Class)}; \
	s_interfaces.push_back( myEntry );

#define EXPOSURE_END_IMPL(_parentclasstype, _parentoffs)\
	PyMethodDef methodsEndItem = { 0 };\
	s_methods.push_back( methodsEndItem ); \
	Be::VarEntry attributesEndItem = { 0 }; \
	s_attributes.push_back( attributesEndItem ); \
	Be::InterfaceEntry interfacesEnd = { 0 }; \
	s_interfaces.push_back( interfacesEnd ); \
	\
	BlueInitializePyType( &s_pyType, &_tmpclsid, _tmpdoc, &s_interfaces[0], PyNew ); \
	\
	static Be::ClassInfo _classinfo; \
	_classinfo.mClassId = &_tmpclsid;  \
	_classinfo.mIID = &s_iid; \
	_classinfo.mDescription = _tmpdoc; \
	_classinfo.mInterfaceTable = &s_interfaces[0]; \
	_classinfo.mMemberTable = &s_attributes[0]; \
	_classinfo.mPyMethodTable = &s_methods[0]; \
	_classinfo.mParentClassInfo = _parentclasstype; \
	_classinfo.mOffsetToParent = _parentoffs; \
	_classinfo.mRtti = nullptr; \
	_classinfo.mTypeObject = &s_pyType; \
	_classinfo.mLiveCount = 0; \
	_classinfo.mLockCount = 0; \
	_classinfo.mFunctionSignatures = &s_signatures; \
	\
	s_classInfo = &_classinfo; \
	return &_classinfo;


// Use this function to cast a Blue-wrapped Python object to a concrete C++ type (Blue class).
// This function is typically used inside a Py C++ wrapper function for a C++ class method.
// Example:
//
// static PyObject* PyFoo( PyObject* self, PyObject* args ) 
// { 
//		Bongo* b = BluePythonCast<Bongo*>( self ); 
//		b->Foo();
//		Py_INCREF(Py_None); return PyNone;
// }
template< class POINTER_TYPE > POINTER_TYPE BluePythonCast( PyObject* self )
{
	return dynamic_cast<POINTER_TYPE>(BlueUnwrapObjectFromPython(self));
}

// Use this function to cast a Blue-wrapped Python object to a concrete C++ type (Blue class).
// This function is typically used inside a Py C++ wrapper function for a C++ class method.
// This is essentially the same as BluePythonCast, making use of the fact that each Blue
// object type has its own Python type object now. Introduced as a separate function
// so we can test it more thoroughly before having it replace BluePythonCast.
// Example:
//
// static PyObject* PyFoo( PyObject* self, PyObject* args ) 
// { 
//		Bongo* b = BluePythonStaticCast<Bongo*>( self ); 
//		b->Foo();
//		Py_INCREF(Py_None); return PyNone;
// }
template< class POINTER_TYPE > POINTER_TYPE BluePythonStaticCast( PyObject* self )
{
	PyTypeObject* type = self->ob_type;
	if( type != std::remove_pointer<POINTER_TYPE>::type::ClassType_()->mTypeObject )
	{
		return NULL;
	}

	BluePythonObject *wrap = static_cast<BluePythonObject *>(self);

	return static_cast<POINTER_TYPE>( wrap->mObj );
}

BLUEIMPORT PyObject* BlueCreateInstanceFromPython( const Be::Clsid& clsid, PyObject* args, PyObject* kwds );

//--------------------------------------------------------------------
// Interface mapping macros
//--------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Single line exposure macros
//////////////////////////////////////////////////////////////////////////

// MAP_METHOD maps a free function with the signature
// PyObject* pyFunc( PyObject* self, PyObject* args )
// as a method on the object in Python
#define MAP_METHOD( nameString, pyFunc, docString )\
{\
	PyMethodDef d = { nameString, &BlueFunctionAsMethodHelper<pyFunc>, METH_VARARGS, docString };\
	s_methods.push_back( d );\
}

#define MAP_METHOD_AND_WRAP( name, functionName, docString ) \
{\
	PyMethodDef d = { name, &BlueMethodHelper<decltype(&_Class::functionName), &_Class::functionName>, METH_VARARGS, docString };\
	s_methods.push_back( d );\
	s_signatures[name] = BlueGetFunctionSignature( &_Class::functionName ); \
}

#define MAP_METHOD_AND_WRAP_OPTIONAL_ARGS( name, functionName, numOptional, docString ) \
{\
	PyMethodDef d = { name, &BlueMethodHelperWithOptionalArgs<decltype(&_Class::functionName), &_Class::functionName, numOptional>, METH_VARARGS, docString };\
	s_methods.push_back( d );\
	s_signatures[name] = BlueGetFunctionSignature( &_Class::functionName, numOptional ); \
}

// MAP_METHOD_AS_METHOD maps a member function with the signature
// PyObject* Class::pyFunc( PyObject* args )
// as a method on the object in Python
#define MAP_METHOD_AS_METHOD( nameString, pyFunc, docString )\
{\
	PyMethodDef d = { nameString, &BlueMethodAsMethodHelper<decltype(&_Class::pyFunc), &_Class::pyFunc>, METH_VARARGS, docString };\
	s_methods.push_back( d );\
}

// MAP_METHOD_WITH_KEYWORD_ARGUMENTS maps a member function with the signature
// PyObject* Class::pyFunc( PyObject* args, PyObject* kwargs )
// as a method on the object in Python
#define MAP_METHOD_WITH_KEYWORD_ARGUMENTS( nameString, pyFunc, docString )\
{ \
	PyMethodDef d = { \
		nameString, \
		reinterpret_cast<PyCFunction>( &BlueMethodWithKeywordArgumentsHelper<decltype( &_Class::pyFunc ), &_Class::pyFunc> ), \
		METH_VARARGS | METH_KEYWORDS, \
		docString }; \
	s_methods.push_back( d ); \
}

////////////////////////////////////////////////////////////////////////////////////
// Use for Vector2, Vector3, Color, D3DXVECTOR4 etc. Give Blue*IID types to help
// Jessica select the right tools for the vector type. MAP_ATTRIBUTE is capable of
// picking the correct IID for the ones that have a dedicated type, Color, Quaternion
// etc, but for instance if you want a direction tool for a Vector3 you need this macro.
#define MAP_ATTRIBUTE_WITH_IID( attributeName, member, docString, _flags, _IID )\
{\
	_Class* temp = NULL;\
	static Be::VarEntry d = MAP_MEMBER( attributeName, VarTypeForVariable<decltype(temp->member)>::type, member, _IID, docString, _flags, NULL );\
	s_attributes.push_back( d );\
}

// Map a property with the given name with a getterFunction and a setterFunction
#define MAP_PROPERTY(nameString, getterFunction, setterFunction, docString)\
{\
	static Be::VarEntry d = { \
		nameString, \
		BlueGetPropertyType<_Class, decltype(&_Class::getterFunction), &_Class::getterFunction>(), \
		0, \
		0, \
		GetBlueReturnTypeIID(&_Class::getterFunction), \
		(docString), \
		Be::READWRITE, \
		nullptr, \
		BluePropertyGetter<_Class, decltype(&_Class::getterFunction), &_Class::getterFunction>, \
		BluePropertySetter<_Class, decltype(&_Class::setterFunction), &_Class::setterFunction> }; \
 	s_attributes.push_back( d );\
}

// Map a read-only property with the given name with a getterFunction
#define MAP_PROPERTY_READONLY(nameString, getterFunction, docString)\
{\
	static Be::VarEntry d = { \
		nameString, \
		BlueGetPropertyType<_Class, decltype(&_Class::getterFunction), &_Class::getterFunction>(), \
		0, \
		0, \
		GetBlueReturnTypeIID(&_Class::getterFunction), \
		(docString), \
		Be::READ, \
		nullptr, \
		BluePropertyGetter<_Class, decltype(&_Class::getterFunction), &_Class::getterFunction>, \
		nullptr }; \
 	s_attributes.push_back( d );\
}

// Map a persisted property with the given name with a getterFunction and a setterFunction
#define MAP_PROPERTY_PERSISTED(nameString, getterFunction, setterFunction, docString)\
{\
	static Be::VarEntry d = { \
		nameString, \
		BlueGetPropertyType<_Class, decltype(&_Class::getterFunction), &_Class::getterFunction>(), \
		0, \
		0, \
		GetBlueReturnTypeIID(&_Class::getterFunction), \
		(docString), \
		Be::READWRITE | Be::PERSIST, \
		nullptr, \
		BluePropertyGetter<_Class, decltype(&_Class::getterFunction), &_Class::getterFunction>, \
		BluePropertySetter<_Class, decltype(&_Class::setterFunction), &_Class::setterFunction> }; \
 	s_attributes.push_back( d );\
}

#ifdef _WIN32
#define BLUE_EXPORTED_INIT
#else
#define BLUE_EXPORTED_INIT __attribute__((visibility("default")))
#endif

// Use this macro to implement a standard module init function
#define BLUE_STANDARD_MODULE_INIT( moduleName ) \
	const char* g_moduleName = #moduleName; \
	PyMODINIT_FUNC BLUE_EXPORTED_INIT CCP_CONCATENATE( init, CCP_CONCATENATE( moduleName, CCP_BUILD_FLAVOR ) )() \
	{ \
		BeClasses->RegisterClasses( BlueRegistration::GetClassRegs() ); \
		PyObject* module = Py_InitModule( CCP_STRINGIZE( CCP_CONCATENATE( moduleName, CCP_BUILD_FLAVOR ) ), NULL ); \
		BlueRegisterToModule( module, \
			BlueRegistration::GetClassRegs(), \
			BlueRegistration::GetFuncRegs(), \
			BlueRegistration::GetEnumRegs(), \
			BlueRegistration::GetTestRegs(), \
			BlueRegistration::GetThunkerRegs(), \
			BlueRegistration::GetFuncSignatures() ); \
		BlueRegisterObjectsToModule( module, BlueRegistration::GetObjectRegs() ); \
		BlueRegisterExceptionsToModule( module, BlueRegistration::GetExceptionRegs() ); \
		PyModule_AddObject( module, "BlueWrapper", (PyObject*)BePyTypePtr ); \
	}

#define BLUE_DECLARE_EXCEPTION_EX( name, ... ) __VA_ARGS__ PyObject* CCP_CONCATENATE( BlueGetException, name )();
#define BLUE_DECLARE_EXCEPTION( name ) BLUE_DECLARE_EXCEPTION_EX( name )

#define BLUE_GET_EXCEPTION( name ) ( CCP_CONCATENATE( BlueGetException, name )() )

#define BLUE_DEFINE_EXCEPTION( name, parent ) \
	PyObject* CCP_CONCATENATE( BlueGetException, name )() \
	{ \
		static auto s_exception = PyErr_NewException( const_cast<char*>( ( std::string( g_moduleName ) + "." #name ).c_str() ), BLUE_GET_EXCEPTION( parent ), nullptr ); \
		return s_exception; \
	} \
	BLUE_REGISTER_EXCEPTION( name, CCP_CONCATENATE( BlueGetException, name ) )
