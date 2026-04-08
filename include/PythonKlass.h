// Copyright © 2026 CCP ehf.

#pragma once
#ifndef _PYTHONKLASS_H_
#define _PYTHONKLASS_H_

#include "BlueTypes.h"
#include "BlueSmartPy.h"

// PythonKlass is a wrapper around a BluePyWrapper python object
// as defined in script/util/blue2py.  This object has a member,
// __namespace__ which is the namespace of the wrapper (pseudo)class.
// __namespace__ thus is the class dictionary of the deco class.
//
// What is more, it is also an instance.  It manages the namespace
// of variables that is declared in the Klass.


class PythonKlass
{
public:
	// constructor.  klass is the BluePyWrapper class instance, which is the decl class.
	PythonKlass(PyObject *klass);
	~PythonKlass();

	// Was initialization successful?
	bool ok() const;
	
	//Get any __guid__ string defined in the Klass.  Borrowed reference.
	bool GetGuid(PyObject** guid = 0) const;
	
	//Should this class be persisted? returns true and a new guid reference
	bool IsPersisted(PyObject **guid = 0) const;
	
	//If the object is to be persisted, returns new references to the instance members
	PyObject *Persist() const;

	//Similar to IsPersisted, but checks the __copytodeco__ arg
	bool IsCopied(PyObject** = 0) const;

	//Returns a list of two lists containing class and instnace attribute names.
	PyObject* GetPythonAttributes();

	//Gets an attribute.  Function objects are returned bound to inst instance.
	PyObject* GetAttr( 
		const char* name, 
		bool* handled,
		PyObject *inst
		);

	//Final getattr, when all regular has failed, invoke a __getattr__ handler.
	PyObject *GetAttrFinal(PyObject *self, const char *name, bool *handled);

	//Gets the string representation of 
	const char *GetAttrStr(const char *name);

	//Sets an attribute.  Only works on instance attributes (variables)
	int SetAttr(
		bool *handled,
		PyObject *self, //the self tho which to bind methods
		const char* name,
		PyObject* v,
		bool force,		// if true, attribute is set even if it didn't exist.
		bool nosetattr  // if true, don't call the __setattr__/__delattr__ dudes dudes.
		);
	int OnSetAttr(
		PyObject *self,
		const char *name,
		PyObject *v);

	//Get a string containing the representation of the object
	PyObject *Repr();

	//Partake in the Python GC sceme
	int PyTraverse(visitproc visit, void *arg);

	//We are bound to 'source'.  Copy and apply to dest
	bool CopyTo(IRoot const *source, IRoot *dest);

	//Create a new deco object for the given IRoot.  Used when loading stuff
	static bool Create(IRoot *target, Be::Clsid const &clsid, PyObject *members);

	PyObject *GetKlass() const { return mKlass.NewRef();}
		
	//get the class instance of a module.class tuple.
	static PyObject *GetClass(const char *name);

	//given a name, looks up the __persistdeco__ of the class, to see if it
	//should be persisted
	static bool IsKlassPersisted(PyObject *klass);

private:
	//look up a method, and rebind if.  If it exists, return true.
	bool LookupMethod(PyObject **m, PyObject *self, const char *name);

private:

	//The klass object, that is, the instance of the metaclass
	BluePy mKlass;
	
	// The instance variables.  These have been declared.
	BluePyDict mVars;

	// a cache of class references
	static BluePyDict sClassDict;
};

#endif // _PYTHONKLASS_H
