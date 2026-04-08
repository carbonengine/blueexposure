// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueExposureMacrosDeprecated_h
#define BlueExposureMacrosDeprecated_h

#if BLUE_WITH_PYTHON

template <class T, class I, class Q>
inline PyObject* PyThunkerThunkMagic(PyObject* self, Q meth, PyObject* args)
{
	IRoot *obj = BlueUnwrapObjectFromPython(self);
	I* pThunked = dynamic_cast<I*>(obj);
	T* pThis = static_cast<T*>(pThunked);
	PyObject *result =(pThis->*meth)(args);
	if (!result)
		result = PyThunkLeave(result);
	return result;
}

template <class T>
inline PyObject* PyThunkMagic(PyObject *self, PyObject *(T::*meth)(PyObject *), PyObject* args)
{
	IRoot *obj = BlueUnwrapObjectFromPython(self);
	T* pThis = dynamic_cast<T *>(obj);
	PyObject *result =(pThis->*meth)(args);
	if (!result)
		result = PyThunkLeave(result);
	return result;
}

#define DECLARE_PYMETHODTHUNK(_method)\
	static PyObject* PyThunk_##_method(PyObject* self, PyObject* args)\
	{\
		return PyThunkerThunkMagic<_Class, _Interface>(self, &_Class::Py##_method, args); \
	}\
	PyObject* Py##_method(PyObject* args)  //the method you define


#define DECLARE_PYMETHOD(_method)\
	static PyObject* PyThunk_##_method(PyObject* self, PyObject* args)\
{\
	return PyThunkMagic<>(self, &Py##_method, args); \
}\
	PyObject* Py##_method(PyObject* args)  //the method you define


// DEPRECATED: Use MAP_METHOD
#define MAPPYTHON(_method, _doc)\
	{ \
		PyMethodDef d = {#_method, PyThunk_##_method, METH_VARARGS, _doc}; \
		s_methods.push_back( d ); \
	}


#define THUNKER_BEGIN() \
	static std::vector<PyMethodDef> s_methods;

#define THUNKER_END() \
	PyMethodDef endEntry = { 0 }; \
	s_methods.push_back( endEntry ); \
	return &s_methods[0];


//--------------------------------------------------------------------
// Class member mapping macros
//--------------------------------------------------------------------

// generic member mapping macros - do not use directly

#define MAPMEMBER(_name, _type, _member, _iid, _desc, _flags, _chooser)\
{\
	_Class* temp = NULL;\
	Be::VarEntry d = {	_name, _type, BLUE_MEMBEROFFSET(_Class, _member), BLUE_MEMBERSIZE(_member), _iid,\
	_desc, _flags, _chooser, nullptr, nullptr};\
	s_attributes.push_back( d ); \
}

#define MAPMEMBERSIZE(_name, _type, _member, _iid, _desc, _flags, _chooser, _size)\
{\
	_Class* temp = NULL;\
	Be::VarEntry d = {	_name, _type, BLUE_MEMBEROFFSET(_Class, _member), _size, _iid,\
	_desc, _flags, _chooser, nullptr, nullptr};\
	s_attributes.push_back( d ); \
}

#define MAPMEMBERFUNC(_name, _type, _func, _desc, _flags)\
{\
	_Class* temp = NULL;\
	Be::VarEntry d = {	_name, _type, (uintptr_t)&(_func), 0, 0, _desc, _flags, nullptr, nullptr, nullptr };\
	s_attributes.push_back( d ); \
}

////////////////////////////////////////////////// //////////////////////////////////
// Use for Vector2, Vector3, Color, D3DXVECTOR4 etc
// For '_interface' pick on of the Blue*IID types (for Jessica handling of the data);
// pick BlueDefaultIID if you just want it to be presented as a float array
#define MAPFLOATARRAY(_name, _member, _interface, _desc, _flags )\
	MAPMEMBER(_name, Be::FLOATARRAY, \
	_member, &_interface, _desc, _flags, NULL)

#define MAPFLOATARRAYSIZE(_name, _member, _interface, _desc, _flags, _size )\
	MAPMEMBERSIZE(_name, Be::FLOATARRAY, \
	_member, &_interface, _desc, _flags, NULL, sizeof(float) * _size)


#endif
#endif // BlueExposureMacrosDeprecated_h