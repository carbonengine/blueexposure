// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BluePythonObject_h
#define BluePythonObject_h

#include <CcpMacros.h>

struct IRoot;
namespace Be
{
	union Var;
	struct VarEntry;
}

struct BluePythonObject : public PyObject
{
	IRoot* mObj;
};

BLUEIMPORT BluePythonObject* BlueWrapObjectForPython( IRoot* obj );
BLUEIMPORT IRoot* BlueUnwrapObjectFromPython( PyObject* obj );

//Convert a blue var into a python object
BLUEIMPORT PyObject* BlueConvertValueToPython( const Be::VarEntry* entry, const Be::Var* value );

//Given a Be type, try to convert the given PyObject into that type
BLUEIMPORT bool BlueConvertValueFromPython( const Be::VarEntry* var, Be::Var* value, PyObject* v );	

extern "C" PyObject* PyExc_BlueError;
extern BLUEIMPORT PyTypeObject *BePyTypePtr;

#endif
