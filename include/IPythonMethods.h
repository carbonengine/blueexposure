// Copyright © 2026 CCP ehf.

#pragma once
#ifndef IPythonMethods_h
#define IPythonMethods_h

#include "BlueTypes.h"

BLUE_INTERFACE(IPythonMethods) : public IRoot
{
	virtual void Destroy(
		) = 0;

	virtual PyObject* GetAttr(
		const char* name,
		bool* handled
		) = 0;

	virtual bool SetAttr(
		const char* name,
		PyObject* v,
		bool* handled
		) = 0;

	virtual PyObject* Repr(
		bool* handled
		) = 0;
};

#endif // IPythonMethods_h