// Copyright © 2026 CCP ehf.

// Implements weak references in blue.  The class provides a template class for weak
// references, as well as declare a class that implements a weak reference site.

#ifndef _BLUEWEAKREF_H_
#define _BLUEWEAKREF_H_

#include <algorithm>
#include "BlueTypes.h"

// An helper that manages weak references to an object that supports them.
class BlueWeakHelper 
{
public:
	BlueWeakHelper() : mRefs( "WeakRefSite/mRefs" ) {};
	
	bool HaveWeakRefs() const {return mRefs.size() != 0;}
	void NotifyWeakRefs(IWeakObject *obj) {
		// Tell all refs that we are leaving, and invalidate the registry

		// It's possible that the callbacks issued here cause refcounts to change
		// and could therefore cause objects in the set to die before we get to them.
		while( !mRefs.empty() )
		{
			IWeakRef* wr = mRefs.back();
			mRefs.pop_back();
			wr->WeakRefNotify( obj );
		}
	}

	// Delegate the IWeakObject interface to these functions.
	void WeakRefRegister(IWeakRef *ref)
	{
		mRefs.push_back(ref);
	}
	
	void WeakRefUnregister(IWeakRef *ref)
	{
		set_t::iterator it = std::find(mRefs.begin(), mRefs.end(), ref);

		// This safeguard prevents a crash resulting from a strange edge case
		// where BluePythonWeakRef object was destroyed in the callback of
		// another BluePythonWeakRef object. Both objects held a weak reference
		// to the same object, but as this happened in the WeakRefNotify
		// the mRefs had been cleared.
		if( it != mRefs.end() )
		{
			mRefs.erase( it );
		}
	}
		
private:
	//We use a vector.  We will typically have very few refs to each
	//object and the vector is very fast and lightweight in that case.
	typedef TrackableStdVector<IWeakRef *> set_t;
	set_t mRefs; //The weakref mapping
};


//A helper class to bring the IWeakObject interface in.
template <class T>
class BlueWeakAdapter : public T, public IWeakObject
{
public:
	//IWeakObject interface delegation:
	void WeakRefRegister(IWeakRef *ref) {mWeak.WeakRefRegister(ref);}
	void WeakRefUnregister(IWeakRef *ref) {mWeak.WeakRefUnregister(ref);}

protected:
	bool HaveWeakRefs() const {
		return mWeak.HaveWeakRefs();
	}
	void NotifyWeakRefs() {
		mWeak.NotifyWeakRefs(this);
	}

private:
	BlueWeakHelper mWeak;  //manages weakref support.
};
	

//Concrete instantiation of an object that adds weakref support
//This class overrides QueryInterface to get IWeakObject
//and Unlock to allow for object resurrection
template <class T>
class RootRefLockWR : public RootRefLock<BlueWeakAdapter<T> >
{
public:
	// IRoot interface modifications
	const Be::ClassInfo* ClassType() const
	{
		return T::ClassType();
	}

	bool QueryInterface(const Be::IID& riid, void** ppv, BLUEQIOPT options)
	{
		//proper handling of the IWeakObject query interface
		static Be::IID weakObjType( "IWeakObject" );
		if( weakObjType.IsEqual( riid ) )
		{
			*ppv = (void*)static_cast<IWeakObject*>(this);
			this->Lock();
			return true;
		}

		return RootRefLock<BlueWeakAdapter<T> >::QueryInterface(riid, ppv, options);
	}

	void Unlock(){
		CCP_ASSERT( this->mLockCount > 0 );
		this->LockIncrement(-1);
		int count = --this->mLockCount;
		//ok, we are going away.  We must repeatedly notify all weakrefs.
		//new weak refs may be added during the weakref notification process
		while(count == 0 && this->HaveWeakRefs()) {
			//temporarily increase the lockcount again while we do the notification.
			//This is required so that a temporary reference created during notification
			//doesn't cause object deletion when it goes out of scope.
			++this->mLockCount;
			this->NotifyWeakRefs();
			count = --this->mLockCount;
		}
		if (count == 0) {
			//it wasn"t resurrected
			this->destroy();
		}
	}
};

template <class T>
class RootRefLockWRShutdown : public RootRefLockWR<T>
{
public:
	virtual void destroy() {
		this->Shutdown();
		RootRefLockWR<T>::destroy();
	}
};


template <class T>
class BlueEmptyWeakAdapter : public T, public IWeakObject
{
public:
	BlueEmptyWeakAdapter() 
	{
	}
	
	BlueEmptyWeakAdapter(IRoot *parent) 
		:T(parent) 
	{
	}
};


template <class T>
class RootParentLockWR : public RootParentLock<BlueEmptyWeakAdapter<T>>
{
public:
	RootParentLockWR( IRoot *lockobj ) 
		:RootParentLock<BlueEmptyWeakAdapter<T>>( lockobj )
	{
	}

	const Be::ClassInfo* ClassType() const
	{
		return T::ClassType();
	}

	void WeakRefRegister( IWeakRef *ref )
	{
		IWeakObject* weak = nullptr;
		if( this->mLockObj->QueryInterface( GetIWeakObjectIID(), reinterpret_cast<void**>( &weak ), BEQI_SILENT ) && weak )
		{
			weak->WeakRefRegister( ref );
			weak->Unlock();
		}
	}
	
	void WeakRefUnregister( IWeakRef *ref ) 
	{
		IWeakObject* weak = nullptr;
		if( this->mLockObj->QueryInterface( GetIWeakObjectIID(), reinterpret_cast<void**>( &weak ), BEQI_SILENT ) && weak )
		{
			weak->WeakRefUnregister( ref );
			weak->Unlock();
		}
	}

	bool QueryInterface( const Be::IID& riid, void** ppv, BLUEQIOPT options )
	{
		if( GetIWeakObjectIID().IsEqual( riid ) )
		{
			*ppv = (void*)static_cast<IWeakObject*>( this );
			this->Lock();
			return true;
		}

		return RootParentLock<BlueEmptyWeakAdapter<T>>::QueryInterface(riid, ppv, options);
	}
};



template <class T>
class RootNoLockWR : public RootNoLock<BlueEmptyWeakAdapter<T>>
{
public:
	const Be::ClassInfo* ClassType() const
	{
		return T::ClassType();
	}

	void WeakRefRegister( IWeakRef* )
	{
	}
	
	void WeakRefUnregister( IWeakRef* )
	{
	}

	bool QueryInterface( const Be::IID& riid, void** ppv, BLUEQIOPT options )
	{
		if( GetIWeakObjectIID().IsEqual( riid ) )
		{
			*ppv = (void*)static_cast<IWeakObject*>( this );
			this->Lock();
			return true;
		}

		return RootNoLock<BlueEmptyWeakAdapter<T>>::QueryInterface(riid, ppv, options);
	}
};

#endif