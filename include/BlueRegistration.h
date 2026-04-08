// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BLUE_REGISTRATION_H
#define BLUE_REGISTRATION_H

#include "BlueTypes.h"
#include "BlueExposureHelpers.h"
#include <vector>
#include <map>

// Class registrations done with BLUE_REGISTER_CLASS end up in a ClassRegsVector
typedef std::vector<Be::ClassRegistration> ClassRegsVector;

// Thunker registrations done with BLUE_REGISTER_THUNKER end up in a ThunkerRegsVector
typedef std::vector<std::pair<const BlueMethodDefinition*, Be::IID>> ThunkerRegsVector;

// Module functions done with MAP_FUNCTION end up in a FuncRegsVector
typedef std::vector<BlueMethodDefinition> FuncRegsVector;

typedef FuncRegsVector TestRegsVector;

typedef std::map<std::string, Be::BlueExposureFunctionSignature> FuncSignatures;

typedef std::vector<std::pair<const char*, IRoot**>> ObjectRegsVector;

//////////////////////
// Enum forward decls
//

enum EnumRegistrationType
{
	// enum values become registered on the module
	// f.ex. trinity.TRIBLENDOP_ADD
	ENUM_REG_VALUES_ON_MODULE = 1,

	// The name of the chooser is registered as a BlueEnum object on the module
	// this object has members (which cannot be modified) for each of the values in the enum
	// f.ex. trinity.TR2STANDARDILLUMINANT.CIE_F12
	ENUM_REG_ENUM_OBJECT_ON_MODULE = 2,
};

// We choose to store a copy of the enums in a vector
typedef std::vector<Be::VarChooser> EnumValues;

// Function pointer type for a function that gets a vector of Enum registrations
typedef EnumValues& (*GetEnumValuesFunctionTypePtr)();

// Typedefs for the map of enumName -> enum values (through a getter function)
typedef std::pair<std::string, std::pair<uint32_t,GetEnumValuesFunctionTypePtr> > EnumTypeRegistrationValues;

// Typedef for the map of enumName -> enum values (through a getter function) + enum registration type
typedef std::map<std::string, std::pair<uint32_t,GetEnumValuesFunctionTypePtr> > EnumRegsMap;
//
//////////////////

#if BLUE_WITH_PYTHON
typedef PyObject* ( *ScriptExceptionGetter )();
#elif BLUE_NO_EXPOSURE
typedef const char* ( *ScriptExceptionGetter )();
#endif

typedef std::map<std::string, ScriptExceptionGetter> ExceptionRegsMap;

#if BLUE_WITH_PYTHON

extern BLUEIMPORT void BlueRegisterToModule( PyObject* module, 
											 const ClassRegsVector& classRegs, 
											 const FuncRegsVector& funcRegs = FuncRegsVector(), 
											 const EnumRegsMap& enumRegs = EnumRegsMap(),
											 const TestRegsVector& testRegs = TestRegsVector(),
											 const ThunkerRegsVector& thunkerRegs = ThunkerRegsVector(),
											 const FuncSignatures& funcSignatures = FuncSignatures() );

extern BLUEIMPORT void BlueRegisterObjectsToModule( PyObject* module, ObjectRegsVector& objectRegs );
extern BLUEIMPORT void BlueRegisterExceptionsToModule( PyObject* module, ExceptionRegsMap& exceptionRegs );

// For registration of standard module functions
void BlueRegisterModuleThunkers( PyObject* module, const ClassRegsVector& classRegs );
void BlueRegisterModuleThunkers( PyObject* module, const Be::ClassRegistration classRegs[], size_t size );

#endif

class BlueRegistration
{
public:
	// The following functions get the vectors of things registered with static
	// initializers, through the various BLUE_REGISTER_XXX macros.
	// These need to be in functions to get around order-of-initialization issues,
	// and they are inlined to ensure that each module gets its own instance
	// of a registration vector when linking against Blue.

	static ClassRegsVector& GetClassRegs()
	{
		static ClassRegsVector s_classRegs;
		return s_classRegs;
	}

	static ThunkerRegsVector& GetThunkerRegs()
	{
		static ThunkerRegsVector s_thunkerRegs;
		return s_thunkerRegs;
	}

	static FuncRegsVector& GetFuncRegs()
	{
		static FuncRegsVector s_funcRegs;
		return s_funcRegs;
	}

	static FuncSignatures& GetFuncSignatures()
	{
		static FuncSignatures s_funcSignatures;
		return s_funcSignatures;
	}

	static EnumRegsMap& GetEnumRegs()
	{
		static EnumRegsMap s_enumRegs;
		return s_enumRegs;
	}

	static TestRegsVector& GetTestRegs()
	{
		static TestRegsVector s_testRegs;
		return s_testRegs;
	}

	static ObjectRegsVector& GetObjectRegs()
	{
		static ObjectRegsVector s_objectRegs;
		return s_objectRegs;
	}

	static ExceptionRegsMap& GetExceptionRegs()
	{
		static ExceptionRegsMap s_exceptionRegs;
		return s_exceptionRegs;
	}


	// For internal use. Note that this functions is not inlined - when linking
	// against Blue, all modules must share this vector. When linking statically
	// it doesn't matter.
	static ThunkerRegsVector& GetGlobalThunkerRegs();
};

/////////////////
// Class registration
//
class ClassRegistrar
{
public:
	ClassRegistrar( const Be::ClassInfo* type, Be::CreateInstance* createFunc, unsigned int flags )
	{
		Be::ClassRegistration cr;
		cr.mType = type;
		cr.mCreateFn = createFunc;
		cr.mFlags = flags;
		BlueRegistration::GetClassRegs().push_back( cr );
	}
};

#define BLUE_REGISTER_CLASS( classname ) \
	static ClassRegistrar s_##classname##ClassRegistrar( classname::ClassType_(), &SimpleFactory<O##classname>::Create, 0)

BLUEIMPORT bool ClassRegistrarNullFactory(const Be::IID& riid, void** ppv);

#define BLUE_REGISTER_CLASS_NO_PYTHON_CONSTRUCTOR( classname ) \
	static ClassRegistrar s_##classname##ClassRegistrar( classname::ClassType_(), &SimpleFactory<O##classname>::Create, Be::ClassRegistration::DISABLE_PYTHON_CONSTRUCTION )


#define BLUE_REGISTER_ABSTRACT_CLASS( classname ) \
	static ClassRegistrar s_##classname##ClassRegistrar( classname::ClassType_(), &ClassRegistrarNullFactory, Be::ClassRegistration::DISABLE_PYTHON_CONSTRUCTION )

#define BLUE_REGISTER_CLASS_EX( classname, factoryFunc, flags ) \
	static ClassRegistrar s_##classname##ClassRegistrar( classname::ClassType_(), &factoryFunc, flags )


/////////////////////////////////////////////////////////////////////////
// Thunker registration
//
class ThunkerRegistrar
{
public:
	ThunkerRegistrar( const BlueMethodDefinition* def, const Be::IID& iid )
	{
		BlueRegistration::GetThunkerRegs().push_back( std::make_pair( def, iid ) );
	}
};

#define BLUE_REGISTER_THUNKER( def, iid ) \
	static ThunkerRegistrar CCP_CONCATENATE( s_thunker_, __COUNTER__ )( def, iid )

/////////////////
// Function registration
//
class FunctionRegistrar
{
public:
	FunctionRegistrar( const char* expname, BlueScriptCFunction func, const char* docstring )
	{
		BlueMethodDefinition md;
		md.ml_name = expname;
		md.ml_meth = func;
#if BLUE_WITH_PYTHON
		md.ml_flags = METH_VARARGS;
#endif
		md.ml_doc = docstring;
		BlueRegistration::GetFuncRegs().push_back( md );
	}
	FunctionRegistrar( const char* expname, BlueScriptCFunction func, const char* docstring, const Be::BlueExposureFunctionSignature& signature )
	{
		BlueMethodDefinition md;
		md.ml_name = expname;
		md.ml_meth = func;
#if BLUE_WITH_PYTHON
		md.ml_flags = METH_VARARGS;
#endif
		md.ml_doc = docstring;
		BlueRegistration::GetFuncRegs().push_back( md );
		BlueRegistration::GetFuncSignatures()[expname] = signature;
	}
};

#define MAP_FUNCTION( _expname, _func, _docstring ) \
	static FunctionRegistrar s_##_func##FunctionRegistrar(_expname, _func, _docstring)

#define MAP_FUNCTION_AND_WRAP( _expname, _func, _docstring ) \
	static FunctionRegistrar s_##_func##FunctionRegistrar(_expname, &BlueFunctionHelper<decltype(&_func), &_func>, _docstring, BlueGetFunctionSignature( &_func ) )


/////////////////
// Enum support
//

BLUEIMPORT std::string GetEnumValueName( GetEnumValuesFunctionTypePtr f, uint32_t val );
BLUEIMPORT std::string GetEnumValueNameAsBitMask( GetEnumValuesFunctionTypePtr f, uint32_t val );

// Template registration for each enum type, allowing:
// EnumRegistration<ENUM>::GetValueName( ... ) in C++
template< typename T > class EnumRegistration
{

public:
	static EnumValues& GetValues()
	{
		static EnumValues values;
		return values;
	}

	static void RegisterValue( const char* valueName, int val, const char* doc )
	{
		EnumValues& vals = GetValues();
		const Be::VarChooser v = { valueName, { BeCast( val ) },  doc };
		vals.push_back( v );
	}

	static const std::string GetValueNameAsBitMask( uint32_t val )
	{
		return GetEnumValueNameAsBitMask( GetValues, val );
	}

	static const std::string GetValueName( uint32_t val )
	{
		return GetEnumValueName( GetValues, val );
	}
};

struct EnumTypeRegistration
{
	template< typename T > EnumTypeRegistration( const char* typeName, 
												 const Be::VarChooser* c, 
												 T t, 
												 uint32_t registrationType )
	{
		std::string s( typeName );
		std::pair<uint32_t,GetEnumValuesFunctionTypePtr> v =  std::make_pair(
			registrationType,
			&EnumRegistration<T>::GetValues );
		BlueRegistration::GetEnumRegs().insert( EnumTypeRegistrationValues( s, v ) );

		while( c->mKey )
		{
			EnumRegistration<T>::RegisterValue( c->mKey, c->mValue.mLong, c->mDescription );
			++c;
		}
	}

	static GetEnumValuesFunctionTypePtr GetTypeValuesGetter( const std::string& typeName )
	{
		EnumRegsMap& regs = BlueRegistration::GetEnumRegs();
		EnumRegsMap::const_iterator find = regs.find( typeName );
		if( find != regs.end() )
		{
			return find->second.second;
		}

		return NULL;
	}
};
// BLUE_REGISTER_ENUM( exposedEnumName, enum, chooser ): Register enums to expose them to Python
#define BLUE_REGISTER_ENUM( _exposedEnumName, _enum, _enumChooser ) \
	static EnumTypeRegistration s_##_enumChooser##Registrar( _exposedEnumName, &_enumChooser[0], _enum(), ENUM_REG_VALUES_ON_MODULE )

#define BLUE_REGISTER_ENUM_EX( _exposedEnumName, _enum, _enumChooser, enumExposureOptions ) \
	static EnumTypeRegistration s_##_enumChooser##Registrar( _exposedEnumName, &_enumChooser[0], _enum(), enumExposureOptions )

class ModuleObjectRegistrar
{
public:
	ModuleObjectRegistrar( const char* name, IRoot** obj )
	{
		BlueRegistration::GetObjectRegs().push_back( std::make_pair( name, obj ));
	}
};

// Register a global pointer to an object to the module under the given name.
// This is applicable to singleton objects, such as BeClasses.
#define BLUE_REGISTER_GLOBAL_AS_MODULE_OBJECT( name, obj ) \
	static ModuleObjectRegistrar CCP_CONCATENATE( s_object_, __COUNTER__ )( name, (IRoot**)&obj )


/////////////////
// Exception registration
//
class ExceptionRegistrar
{
public:
	ExceptionRegistrar( const char* name, ScriptExceptionGetter getExceptionFunction )
	{
		BlueRegistration::GetExceptionRegs()[name] = getExceptionFunction;
	}
};

#define BLUE_REGISTER_EXCEPTION( name, exceptionGetter ) static ExceptionRegistrar s_##name##Registar( #name, exceptionGetter );

#endif // BLUE_REGISTRATION_H