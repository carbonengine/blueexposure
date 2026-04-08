// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueScriptValue_h
#define BlueScriptValue_h

#if BLUE_WITH_PYTHON

#include <Python.h>

typedef PyObject* BlueScriptValue;
typedef PyObject* BlueScriptArguments;
#define BLUE_SCRIPT_ERROR nullptr
#define BLUE_SCRIPT_NO_RETURN_VALUE Py_RETURN_NONE
typedef PyObject* (*BluePropertyGetterFunction)(PyObject*);
typedef PyObject* (*BluePropertySetterFunction)(PyObject*, PyObject*);
typedef PyMethodDef BlueMethodDefinition;
typedef PyCFunction BlueScriptCFunction;

#elif BLUE_NO_EXPOSURE

typedef void* BlueScriptValue;
typedef void* BlueScriptArguments;
#define BLUE_SCRIPT_ERROR nullptr
#define BLUE_SCRIPT_NO_RETURN_VALUE return nullptr
typedef void (*BluePropertyGetterFunction)();
typedef void (*BluePropertySetterFunction)();
typedef void (*BlueScriptCFunction)();
struct BlueMethodDefinition {
	const char* ml_name;		/* The name of the built-in function/method */
	BlueScriptCFunction ml_meth;	/* The C function that implements it */
	const char* ml_doc;			/* The __doc__ attribute, or NULL */
};

#endif

BLUEIMPORT struct IRoot* BlueUnwrapObjectFromScriptValue( BlueScriptValue sv );

#endif // BlueScriptValue_h