// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueMemberFunctionAsCallable_h
#define BlueMemberFunctionAsCallable_h

#include "BlueFunctionTraits.h"


// Define a templated structure for wrapping a member function pointer as a callable
// that can be used with the rest of the system where function pointers are expected.
// This is essentially a bound member function, binding a pointer to an object and a
// member function pointer.
// The template is specialized below for 0 to 9 parameters, const and non-const.
template<typename Function>
struct BlueMemberFunctionAsCallable;

template<typename Function, int numOpt>
struct BlueFunctionWithOptionalArguments
{
	static const int numOptional = numOpt;
};

template<typename Function, int numOpt>
struct BlueFunctionWithOptionalArguments< BlueMemberFunctionAsCallable< Function >, numOpt > : public BlueMemberFunctionAsCallable< Function >
{
	static const int numOptional = numOpt;

	template< typename ClassType >
	BlueFunctionWithOptionalArguments( ClassType* o, Function f ) : BlueMemberFunctionAsCallable< Function >( o, f ) {}
};

template<typename ClassType, typename ReturnType>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)() >
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)();

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()()
	{
		return (obj->*func)();
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)() const >
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)() const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()()
	{
		return (obj->*func)();
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1 )
	{
		return (obj->*func)( a1 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1 )
	{
		return (obj->*func)( a1 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2 )
	{
		return (obj->*func)( a1, a2 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2 )
	{
		return (obj->*func)( a1, a2 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3 )
	{
		return (obj->*func)( a1, a2, a3 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3 )
	{
		return (obj->*func)( a1, a2, a3 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4 )
	{
		return (obj->*func)( a1, a2, a3, a4 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4 )
	{
		return (obj->*func)( a1, a2, a3, a4 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6 ) const >
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6, A7 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6, A7 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6, a7 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6, A7 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6, A7 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6, a7 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6, A7, A8 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6, A7, A8 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6, a7, a8 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6, A7, A8 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6, A7, A8 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6, a7, a8 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6, A7, A8, A9 )>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6, A7, A8, A9 );

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
	}

	ClassType* obj;
	Function func;
};

template<typename ClassType, typename ReturnType, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
struct BlueMemberFunctionAsCallable< ReturnType (ClassType::*)( A1, A2, A3, A4, A5, A6, A7, A8, A9 ) const>
{
	// Define the type 'Function' for storing the function pointer below
	typedef ReturnType (ClassType::*Function)( A1, A2, A3, A4, A5, A6, A7, A8, A9 ) const;

	BlueMemberFunctionAsCallable( ClassType* o, Function f ) : obj( o ), func( f ) {}

	ReturnType operator()( A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9 )
	{
		return (obj->*func)( a1, a2, a3, a4, a5, a6, a7, a8, a9 );
	}

	ClassType* obj;
	Function func;
};


#endif // BlueMemberFunctionAsCallable_h