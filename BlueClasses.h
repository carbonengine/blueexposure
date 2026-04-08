// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueClasses_h
#define BlueClasses_h

#include "include/BlueExposureMacros.h"
#include "include/IBlueClasses.h"
#include "include/BlueUtil.h"

#ifndef NDEBUG
// Keep track of all live instances of IRoot objects
#define BLUE_LIVELIST_ENABLED 0
#endif

BLUE_DECLARE( BlueClasses );

BLUE_CLASS( BlueClasses ) : public IBlueClasses
{
public:
	EXPOSE_TO_BLUE();

	// Constructor is private

	static bool Initialize();

	//////////////////////////////////////////////////////////////////////////
	// IBlueClasses
	
	// Register classes from a null-terminated vector of class registrations
	void RegisterClasses( const Be::ClassRegistration table[] ) override;

	// Register classes from a std::vector of class registrations (null-termination
	// not required)
	void RegisterClasses( std::vector<Be::ClassRegistration>& table	) override;

	// Unregister classes from a null-terminated vector of class registrations
	void UnregisterClasses(	const Be::ClassRegistration table[] ) override;

	// Unregister classes from a std::vector of class registrations (null-termination
	// not required)
	void UnregisterClasses(	std::vector<Be::ClassRegistration>& table ) override;

	// Get a class registration from a class ID.
	const Be::ClassRegistration* GetClassRegistration( const Be::Clsid& clsid ) override;

	// Find a class ID from the class module and name.
	bool FindClsid( Be::Clsid& clsid, const char* module, const char* name ) override;
	const Be::Clsid* FindClsid( const char* name ) override;

	// Create an instance of the class from the class ID, cast to the given interface.
	bool CreateInstance( const Be::Clsid& clsid, const Be::IID& riid, void** ppv ) override;
	bool CreateInstanceFromName( const char* className, const Be::IID& riid, void** ppv ) override;

	// Works like IUnknown::QueryInterface
	bool QueryThisInterface(
		void* pThis,
		const Be::IID& riid, 
		void** ppv,
		const Be::ClassInfo* type,
		BLUEQIOPT options = BEQI_NONE
		) override;

	// Another convenience function
	const Be::VarEntry* FindVariable(
		const char* name, 
		const Be::ClassInfo* type,
		ptrdiff_t* xtraoffset
		) override;

	void UpdateObjectCount( IRoot* obj, int inst, int lock ) override;

	// Copies data from 'source' into '*dest'. If '*dest' is NULL, a new 
	// instance of same class as 'source' is created. If '*dest' is valid,
	// the data is copied into the object.
	bool CopyTo(
		IRoot* source,
		IRoot** dest,
		ICopier::CopyOverrideCallback copyOverride = nullptr,
		void* overrideContext = nullptr,
		ICopier::PostCopyCallback postCopy = nullptr,
		void* postCopyContext = nullptr
		) override;

	// Same as CopyTo, except that it preserves topology of multiply instanced
	// Child objects.  (CopyTo will resolve such multiple instances in the source
	// object into separate copies in the destination)
	bool CloneTo(
		IRoot* source,
		IRoot** dest
		) override;

	// Process pending deletes. This may be throttled back to limit the time spent
	// in one call.
	void ProcessPendingDeletes() override;

	// Process all pending deletes, without regard for the time taken.
	void ProcessAllPendingDeletes() override;

	void SetPendingDeletesEnabled( bool b ) override;
	bool IsPendingDeletesEnabled() const override;

	// Register a set of methods that are added to any object with the given interface
	void RegisterThunker(
		const BlueMethodDefinition* defs,
		const Be::IID& riid
		) override;
	
	IBlueRtti* GetRtti( const Be::ClassInfo *ci ) override;
	//
	//////////////////////////////////////////////////////////////////////////

protected:
	BlueClasses( IRoot* lockobj = NULL );
	~BlueClasses();

private:

	// generic thunkers
	struct GenericThunker
	{
		const BlueMethodDefinition* mDefs;
		const Be::IID* mIID;
	};

	typedef TrackableStdVector<GenericThunker> Thunkers;
	typedef Thunkers::iterator ThunkIt;
	Thunkers mGenericThunkers;


	// Wrapper around the Be::Clsid pointer. Without this we can't guarantee
	// the Clsid isn't copied around when used in the map.
	struct CIDP {
		CIDP(const Be::Clsid *id) : mID(id) {}
		bool operator < (const CIDP &other) const {return *mID < *other.mID;}
		const Be::Clsid *mID;
	};
	typedef TrackableStdMap<CIDP, const Be::ClassRegistration*> ClassReg;
	typedef ClassReg::iterator ClassRegIt;
	ClassReg m_classes;

	typedef TrackableStdHashMap<std::string, const Be::ClassRegistration*> ClassNameReg;
	typedef ClassNameReg::iterator ClassNameRegIt;
	ClassNameReg m_classesByName;

	void RegisterSingleClass( const Be::ClassRegistration* cr );
	void UnregisterSingleClass( const Be::ClassRegistration* cr );
	const Be::ClassRegistration* GetClassRegImpl( const Be::Clsid& clsid );

	// Maximum time, in milliseconds, to spend on processing pending deletes
	float m_maxTimeForPendingDeletes;

	// Upper limit on number of pending deletes
	unsigned int m_maxPendingDeletes;
	
#if BLUE_WITH_PYTHON
	static PyObject* PyCreateInstance( PyObject* self, PyObject* args );
	static PyObject* PyGetClassTypes( PyObject* self, PyObject* args );
	static PyObject* PyLiveCount( PyObject * self, PyObject * args );
	static PyObject* PyLockCount( PyObject * self, PyObject * args );
	static PyObject* PyGetWrapperList( PyObject * self, PyObject * args );

#endif

	std::string GetPersistedClassesReport();

#if BLUE_LIVELIST_ENABLED
	// We want to keep a list of all the objects alive:
	struct AliveInstEntry {
		AliveInstEntry() : mLock(0) {}
		int mLock;
	};
	typedef TrackableStdMap<IRoot *, AliveInstEntry> AliveByInst_t;
	typedef AliveByInst_t::iterator AliveByInst_i;
	AliveByInst_t mAliveByInst;

	// Objects may be created and destroyed on differed threads
	mutable CcpMutex m_aliveTrackingMutex;

	static PyObject* PyLiveList( PyObject* self, PyObject* args );
#endif

};

TYPEDEF_BLUECLASS( BlueClasses );

#endif
