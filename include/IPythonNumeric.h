// Copyright © 2026 CCP ehf.

#pragma once
#ifndef IPythonNumeric_h
#define IPythonNumeric_h

#include "BlueTypes.h"

//////////////////////////////////////////////////////////////////////
//
// IPythonNumeric interface - optional numeric operators
//
//	Note that only ops marked as 'supported' are implemented
//
//////////////////////////////////////////////////////////////////////
enum PYNUMERIC_OPS
{
	// binary ops
	PYOP_ADD		= 1,	// supported
	PYOP_SUB		= 2,	// supported
	PYOP_MUL		= 3,	// supported
	PYOP_DIV		= 4,	// supported
	PYOP_REM		= 5,
	PYOP_DIVMOD		= 6,
	PYOP_LSHIFT		= 7,
	PYOP_RSHIFT		= 8,
	PYOP_AND		= 9,
	PYOP_XOR		= 10,
	PYOP_OR			= 11,


	// unary ops
	PYOP_NEG		= 21,	// supported
	PYOP_POS		= 22,	// supported
	PYOP_ABS		= 23,
	PYOP_INV		= 24,
	PYOP_INT		= 25,
	PYOP_LONG		= 26,
	PYOP_FLOAT		= 27,
	PYOP_OCT		= 28,
	PYOP_HEX		= 29,

	// ternary ops
	PYOP_POW		= 31,

	// coercion
	PYOP_COERCE		= 41,	// supported
};

BLUE_INTERFACE(IPythonNumeric) : public IRoot
{
	// If the functions return 'false', the op is not supported.
	// '*retval' is always set to NULL prior to call, and if it's
	// NULL upon return, an error MUST be set.

	virtual bool BinaryOp(
		PYNUMERIC_OPS op,
		IRoot* other,
		PyObject** retval
		) = 0;

	virtual bool UnaryOp(
		PYNUMERIC_OPS op,
		PyObject** retval
		) = 0;

	virtual void Coercion(
		PyObject* from,
		PyObject** to
		) = 0;
};


#endif
