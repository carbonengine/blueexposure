// Copyright © 2026 CCP ehf.

#pragma once
#ifndef IBlueClasses_h
#define IBlueClasses_h

#include "BlueTypes.h"
#include "ICopier.h"

#include <vector>

BLUE_INTERFACE_EXPORT( IBlueClasses ) : public IRoot
{
	// Register classes from a null-terminated vector of class registrations
	virtual void RegisterClasses( const Be::ClassRegistration table[] ) = 0;

	// Register classes from a std::vector of class registrations (null-termination
	// not required)
	virtual void RegisterClasses( std::vector<Be::ClassRegistration>& table	) = 0;

	// Unregister classes from a null-terminated vector of class registrations
	virtual void UnregisterClasses(	const Be::ClassRegistration table[] ) = 0;

	// Unregister classes from a std::vector of class registrations (null-termination
	// not required)
	virtual void UnregisterClasses(	std::vector<Be::ClassRegistration>& table ) = 0;

	// Get a class registration from a class ID.
	virtual const Be::ClassRegistration* GetClassRegistration( const Be::Clsid& clsid ) = 0;

	// Find a class ID from the class module and name. This function copies the clsid
    // into the one given.
	virtual bool FindClsid( Be::Clsid& clsid, const char* module, const char* name ) = 0;

    // Find a class ID from the class name, returning a pointer to the centrally
    // registered one.
	virtual const Be::Clsid* FindClsid( const char* name ) = 0;

	// Create an instance of the class from the class ID, cast to the given interface.
	virtual bool CreateInstance( const Be::Clsid& clsid, const Be::IID& riid, void** ppv ) = 0;

    // Create an instance of the object of the given class, cast to the given interface.
	virtual bool CreateInstanceFromName( const char* className, const Be::IID& riid, void** ppv ) = 0;

	// Works like IUnknown::QueryInterface
	virtual bool QueryThisInterface(
		void* pThis,
		const Be::IID& riid, 
		void** ppv,
		const Be::ClassInfo* type,
		BLUEQIOPT options = BEQI_NONE
		) = 0;

	// Another convenience function
	virtual const Be::VarEntry* FindVariable(
		const char* name, 
		const Be::ClassInfo* type,
		ptrdiff_t* xtraoffset
		) = 0;

	// Keep track of objects created/destroyed
	virtual void UpdateObjectCount( IRoot* obj, int inst, int lock ) = 0;

	// Copies data from 'source' into '*dest'. If '*dest' is NULL, a new 
	// instance of same class as 'source' is created. If '*dest' is valid,
	// the data is copied into the object.
	virtual bool CopyTo(
		IRoot* source,
		IRoot** dest,
		ICopier::CopyOverrideCallback copyOverride = nullptr,
		void* overrideContext = nullptr,
		ICopier::PostCopyCallback postCopy = nullptr,
		void* postCopyContext = nullptr
		) = 0;

	// Same as CopyTo, except that it preserves topology of multiply instanced
	// Child objects.  (CopyTo will resolve such multiple instances in the source
	// object into separate copies in the destination)
	virtual bool CloneTo(
		IRoot* source,
		IRoot** dest
		) = 0;

	// Process pending deletes. This may be throttled back to limit the time spent
	// in one call.
	virtual void ProcessPendingDeletes() = 0;

	// Process all pending deletes, without regard for the time taken.
	virtual void ProcessAllPendingDeletes() = 0;

	virtual void SetPendingDeletesEnabled( bool b ) = 0;
	virtual bool IsPendingDeletesEnabled() const = 0;

	// Register a set of methods that are added to any object with the given interface
	virtual void RegisterThunker(
		const BlueMethodDefinition* defs,
		const Be::IID& riid
		) = 0;

	virtual IBlueRtti* GetRtti( const Be::ClassInfo *ci ) = 0;
};

// BlueLockData is a structure that is prefixed to all Blue objects when they're created.
// This is to associate the optional PythonKlass and BlueWrapper structures to the Blue
// object.
struct BlueLockData
{
	class PythonKlass* mPythonKlass;
	class BlueWrapper* mWrapper;
};

// Internal function used by the template code for creating a Blue object
BLUEIMPORT void* BlueInternalCreate( size_t size, const char* name );

// Internal function used the templace code for freeing a Blue object
BLUEIMPORT void BlueInternalFreeObject( void* ptr );

// Internal function used by the template code
BLUEIMPORT bool BlueAddPendingDelete( IRoot* obj );

BlueLockData* BlueInternalGetLockData( void* ptr );
BlueLockData* BlueInternalHasLockData( IRoot* obj );

extern BLUEIMPORT IBlueClasses* BeClasses;

template <typename T>
class BlueObjectHasGetRawRoot
{
	typedef char one;
	struct two
	{
		char x[2];
	};

	template <typename C>
	static one test( decltype( &C::GetRawRoot ) );
	template <typename C>
	static two test( ... );

public:
	enum
	{
		value = sizeof( test<T>( 0 ) ) == sizeof( char )
	};
};


template <typename T>
[[nodiscard]] inline IRootPtr BlueCopy( const T* source, ICopier::CopyOverrideCallback copyOverride = nullptr, void* overrideContext = nullptr, ICopier::PostCopyCallback postCopy = nullptr, void* postCopyContext = nullptr )
{
	IRootPtr dest;
	IRoot* src;
	if constexpr( BlueObjectHasGetRawRoot<T>::value )
	{
		src = source->GetRawRoot();
	}
	else
	{
		src = source->GetRootObject();
	}
	BeClasses->CopyTo( src, (IRoot**)&dest, copyOverride, overrideContext, postCopy, postCopyContext );

	return dest;
}

template <typename T>
[[nodiscard]] inline IRootPtr BlueCopy( const BlueBasicPtr<T>& source, ICopier::CopyOverrideCallback copyOverride = nullptr, void* overrideContext = nullptr, ICopier::PostCopyCallback postCopy = nullptr, void* postCopyContext = nullptr )
{
	IRootPtr dest;
	IRoot* src;
	if constexpr( BlueObjectHasGetRawRoot<T>::value )
	{
		src = source->GetRawRoot();
	}
	else
	{
		src = source->GetRootObject();
	}
	BeClasses->CopyTo( src, (IRoot**)&dest, copyOverride, overrideContext, postCopy, postCopyContext );

	return dest;
}

#endif
