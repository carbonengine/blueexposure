// Copyright © 2026 CCP ehf.

/* 
	*************************************************************************

	BlueUtil.h

	Author:    Kristj�n Valur J�nsson
	Created:   May. 2005
	OS:        Win32
	Project:   Blue

	Description:   

		Macros and class templates to construct complete classes in blue.

	Dependencies:

		Blue

	(c) CCP 2005

	*************************************************************************
*/

#ifndef BLUECLASS_H
#define BLUECLASS_H

#include "IBlueClasses.h"
#include "BlueSmartPtr.h"

//Implement the QueryInterface and ClassInfo part of IRoot
template <class T>
class RootQIImpl : public T
{
public:
	RootQIImpl(IRoot *parent) : T(parent) {}
	RootQIImpl() {}
	bool QueryInterface(const Be::IID& riid, void** ppv, BLUEQIOPT options)	{
		return BeClasses->QueryThisInterface(this, riid, ppv, T::ClassType_(), options);
	}

	IRoot* GetRootObject() const
	{
		// These cast gymnastics resolve the multiple inheritance issues with
		// all interfaces inheriting from IRoot. Note that the key is to have this
		// inside a virtual function.
		void* p = (void*)this;
		return static_cast<IRoot*>( p );
	}
};


// This function is called from the RootRefLock template to determine if
// delayed deletes are allowed for its class. The default is no - classes
// that allow delayed deletes should provide a specialization that returns
// true - use the macro below.
template<typename C> bool BlueClassAllowDelayedDelete()
{
	return false;
};

#define BLUE_CLASS_ALLOW_DELAYED_DELETE(C) \
template<> inline bool BlueClassAllowDelayedDelete<C>() \
{ \
	return true; \
}


//does regular instance locking
template <class T>
class RootRefLock : public RootQIImpl<T>
{
public:
	RootRefLock() : mLockCount(0) {
		InstIncrement(1);  //count new instance
		Lock();
	}
protected:
	virtual ~RootRefLock() {
		CCP_ASSERT(mLockCount<=0);
		InstIncrement(-1);
	}
public:
	
	void Lock()
	{
		CCP_ASSERT( mLockCount >= 0 );
		mLockCount++;
		LockIncrement( 1 );
	}
	void Unlock()
	{
		CCP_ASSERT( mLockCount > 0 );
		LockIncrement(-1);
		if( --mLockCount == 0 )
		{
			destroy();
		}
	}
	long GetFlags()
	{
		return 0;
	}

	int GetRefCount() const
	{ 
		return mLockCount;
	}

	void InstIncrement( int inc )
	{
		if( BeClasses )
		{
			//count instances
			BeClasses->UpdateObjectCount( this->GetRootObject(), inc, 0 );
		}
	}
#ifndef NDEBUG
	void LockIncrement( int inc )
	{
		if( BeClasses )
		{
			//count locks
			BeClasses->UpdateObjectCount( this->GetRootObject(), 0, inc );
		}
	}
#else
	void LockIncrement( int inc )
	{
	}
#endif
	
	//Placement new, used for the internal create
	void* operator new( size_t size )
	{
		const char* name = T::Clsid()->GetName();
		return BlueInternalCreate( size, name );
	}

	//special placement delete, required for exception handling
	void operator delete(void* ptr)
	{
		BlueInternalFreeObject( ptr );
	}

	//The proper destructor.
	virtual void destroy()
	{
		CCP_ASSERT( mLockCount == 0 );

		if( !BlueClassAllowDelayedDelete<T>() || !BlueAddPendingDelete( this->GetRootObject() ) )
		{
			FinalDelete();
		}
	}

	virtual void FinalDelete()
	{
		this->~RootRefLock();
		BlueInternalFreeObject( this );
	}

	std::atomic<int> mLockCount;
};


template <class T>
class RootRefLockShutdown : public RootRefLock<T>
{
public:
	virtual void destroy() {
		this->Shutdown();
		RootRefLock<T>::destroy();
	}
};

//locking that delegates locking to its parent.  Use for auto objects or statics.
template <class T>
class RootParentLock : public RootQIImpl<T>
{
public:
	RootParentLock(IRoot *lockobj) : RootQIImpl<T>(lockobj), mLockObj(lockobj)
	{
		CCP_ASSERT(lockobj);
	}
	void Lock() {mLockObj->Lock();}
	void Unlock() {mLockObj->Unlock();}
	int GetRefCount() const { return mLockObj->GetRefCount(); }
	long GetFlags() {return BLUERT_AUTOVAR;}
	virtual void FinalDelete() {}
	//evil assignment operator, sadly required by the evil BlueListC class
	//assign parents, keep lockobj intact
	RootParentLock<T>& operator=( const RootParentLock<T> &other ) 
	{
		*static_cast<RootQIImpl<T>*>( this ) = *static_cast<const RootQIImpl<T>*>( &other );
		return *this;
	}
	IRoot * const mLockObj;
};


//A special no locking dude, for statics only.
template<class T>
class RootNoLock : public RootQIImpl<T>
{
public:
	void Lock() {}
	void Unlock() {}
	long GetFlags() {return BLUERT_AUTOVAR;}
	int GetRefCount() const { return 0; }
	virtual void FinalDelete() {}
};

template<class T>
class RootNoLockNoQI : public T
{
public:
	void Lock() {}
	void Unlock() {}
	long GetFlags() {return BLUERT_AUTOVAR;}
	int GetRefCount() const { return 0; }
	const Be::ClassInfo* ClassType()
	{
		return T::ClassType();
	}
	bool QueryInterface(const Be::IID& riid, void** ppv, BLUEQIOPT options)
	{
		return false;
	}
};

// use this to initialize parentlocked members in your class initializer list
// It is an idiom used in these Blue classes to initialize parent classes
// with a this pointer to ourselves, in the constructor.  This causes a warning
// which we hereby disable:
#ifdef _MSC_VER
#pragma warning (disable:4355)
#endif
// This is the new and improved PARENTLOCK - the ellipsis are just there for
// backwards compatibility with the old usage where you had to specify an IRoot base class.
// In the new usage you just specify the member object to lock - that's it!
#define PARENTLOCK(obj, ...) \
	obj(lockobj ? lockobj : this->GetRawRoot() )

// DEPRECATED: Don't use this, only here to support old code.  Use the PARENTLOCK macro.
#define PARENTLOCK2(obj, cl) \
	obj(static_cast<cl*>(this))

//The default final class types. Note that this includes support for weak references
#define TYPEDEF_BLUECLASS(Class)\
	typedef RootNoLockWR<Class> C##Class;\
	typedef RootParentLockWR<Class> P##Class;\
	typedef RootRefLockWR<Class> O##Class;

//Final class types without weak ref support.
#define TYPEDEF_BLUECLASS_NO_WEAKREF(Class)\
	typedef RootNoLock<Class> C##Class;\
	typedef RootParentLock<Class> P##Class;\
	typedef RootRefLock<Class> O##Class;

//Final class types, calls Shutdown before destructor
#define TYPEDEF_BLUECLASS_SHUTDOWN(Class)\
	typedef RootNoLockWR<Class> C##Class;\
	typedef RootParentLockWR<Class> P##Class;\
	typedef RootRefLockWRShutdown<Class> O##Class;

//Default setup now includes weak ref support - this macro is now deprecated
#define TYPEDEF_BLUECLASS_WR(Class)\
	TYPEDEF_BLUECLASS(Class)

//Final class types with weak ref support, calls Shutdown before destruction
#define TYPEDEF_BLUECLASS_WR_SHUTDOWN(Class)\
	typedef RootNoLockWR<Class> C##Class;\
	typedef RootParentLockWR<Class> P##Class;\
	typedef RootRefLockWRShutdown<Class> O##Class;


//Class factories, for use with the BlueCreateInstance mechanism of creating class objects.
template <class T>
class SimpleFactory
{
public:
	static bool Create(const Be::IID& riid, void** ppv)
	{
		T* obj = new T;
		bool ok = obj->QueryInterface(riid, ppv, BEQI_NONE);
		if (ok) {
			obj->Unlock();
			//can't assert on lockcount because some classes
			//(TriDevice for example) magically create an
			//extra reference in a global during construction
			//ASSERT(obj->mLockCount == 1);
		}
		return ok;
	};	
};

template <class T>
class SingletonFactory
{
public:
	static bool Create(const Be::IID& riid, void** ppv)
	{
		static T obj;
		return obj.QueryInterface(riid, ppv, BEQI_NONE);
	};	
};

//Use this for proper singletons.  It uses a weakref, so while the single variable
//exists, it will always return the same dude!
template <class T>
class DynamicSingletonFactory
{
public:
	static bool Create(const Be::IID& riid, void** ppv)
	{
		static BlueWeakRef<IRoot> wr;
		*ppv = 0;
		if (wr) {
			return wr->QueryInterface(riid, ppv, BEQI_NONE);
		} else {
			//Create the object so that it can exist in the ROT.  Ugly hack, truly
			T *tmp = new T;
			if (tmp) {
				IRootPtr ir = reinterpret_cast<IRoot*>( tmp );
				wr = static_cast<IRoot*>( ir );
				if (!wr)
					//the object doesn't support weak refs!
					return false;
				return wr->QueryInterface(riid, ppv, BEQI_NONE);
			}
		} 
		return false;
	}
};
	
#endif // BLUECLASS_H
