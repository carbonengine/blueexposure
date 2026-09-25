////////////////////////////////////////////////////////////////////////////////
//
// Creator:		Snorri Sturluson
// Created:		April 2013
// Copyright (c) 2026 CCP Games
//
// Originally developed by Matthias Gudmundsson.
// Maintained by Snorri Sturluson
//
//	Smart pointers - very much like ATL's.

#ifndef _BLUESMARTPTR_H_
#define _BLUESMARTPTR_H_

#include <CcpMacros.h>
#include <CcpAssert.h>
#include "BlueScriptValue.h"

//Utility functions defined in blueutil.cpp.  Fixed functions like these reduce code.
BLUEIMPORT IRoot* BluePtrAssign(IRoot** pp, IRoot* lp);
BLUEIMPORT IRoot* BlueQIPtrAssign(
	IRoot** pp, IRoot* lp,
	const Be::IID& riid,
	BLUEQIOPT options = BEQI_NONE
	);
BLUEIMPORT bool BlueCreateInstance(
	const Be::Clsid& clsid, const Be::IID& riid, void** ppv);

inline IRoot *_BlueQIPtrAssign(IRoot** pp, IRoot* lp, const Be::IID& riid, BLUEQIOPT options = BEQI_NONE)
{
	IRoot *np = 0;
	if( lp )
	{
		lp->QueryInterface( riid, (void**)&np, options );
	}
	IRoot *tmp = *pp;
	*pp = 0;
	if( tmp )
	{
		tmp->Unlock();
	}
	return *pp = np;
}

inline bool _BlueIsSame(IRoot *a, IRoot *b)
{
	if( !a || !b )
	{
		return false;
	}

	IRoot* va = a->GetRootObject();
	IRoot* vb = b->GetRootObject();

	return va == vb;
}
	

//Core BluePtr implementation
//Special inherited class with private Lock, for the -> operator
	template <class T>
	class _NoLock : public T
	{
		private:
			void Lock();
			void Unlock();
	};

template< class T >
class BlueBasicPtr
{
	typedef BlueBasicPtr<T> Class;
public:
	BlueBasicPtr<T>() :  p( nullptr )
	{
	}
	
	BlueBasicPtr<T>(T* lp) :  p( lp )
	{
		Lock();
	}

	BlueBasicPtr<T>( const Class &other ) : p( other.p )
	{
		Lock();
	}


	~BlueBasicPtr()
	{
		Unlock();
	}

	//Conversions:
	operator T*() const
	{
		return p;
	}
	
	// The assert on operator& usually indicates a bug.  If the pointer is non-null the 
	// surrounding code is possibly overwriting the value of p and causing a memory leak.
	// The solution is usually to call Unlock() on the smart pointer before using this operator.
	// If this is really what is needed, however, take the address of the p member explicitly
	// instead of using this operator.
	T** operator&()
	{
		CCP_ASSERT( !p );
		return &p;
	}

	// Return a special subclass, so that people don't accidentally do Class->Unlock()
	_NoLock<T>* operator->() const
	{
		CCP_ASSERT( p );
		return reinterpret_cast<_NoLock<T>*>( p );
	}

	//Assignment.  Note how we return T, and not Class
	T* operator=( T* lp )
	{
		if (lp)
		{
			reinterpret_cast<IRoot*>( lp )->Lock();
		}
		IRoot *tmp = reinterpret_cast<IRoot*>( p );
		if( p )
		{
			p = 0;
			tmp->Unlock();
		}

		return p = lp;
	}

	//Copy assignment.  Required for robustness
	Class& operator=( const Class &other )
	{
		*this = other.p;
		return *this;
	}
	
	Class& operator=( Class &&other )
	{
		if( this->p == other.p )
		{
			return *this;
		}
		this->p = other.p;
		other.p = nullptr;
		return *this;
	}
	
	//Some simple operators, for truth testing and sorting
	bool operator!() const
	{
		return !p;
	}

	bool operator<(T* pT) const
	{
		return p < pT;
	}

	bool operator==(T* pT) const
	{
		return p == pT;
	}

	bool IsEqualObject(T *other) const
	{
		return _BlueIsSame(this->p, other);
	}

	//Convenience functions
	// Create an instance of a class with the given Clsid.
	bool CreateInstance(const Be::Clsid& clsid )
	{
		CCP_ASSERT(!this->p);
		return BlueCreateInstance(clsid, GetIRootIID(), (void**) &(*this));
	}

	// Create an instance of the named class
	bool CreateInstance(const char* clsidstr)
	{
		Be::Clsid clsid;
		return clsid.InitFromString(clsidstr) && CreateInstance(clsid);
	}

protected: //only we do this.
	void Lock()
	{
		if( p )
		{
			reinterpret_cast<IRoot*>( p )->Lock();
		}
	}

public:
	// Release the object, nulling out this smart pointer
	void Unlock()
	{
		//careful to null p before unlocking, to avoid side effects.
		T *pTmp = p;
		if( pTmp )
		{
			p = 0;
			reinterpret_cast<IRoot*>( pTmp )->Unlock();
		}
	}

	// Attach on object to this smart pointer without adding a reference
	void Attach( T* p2 )
	{
		Unlock();
		p = p2;
	}

	// Detaches the object from this smart pointer, with removing a reference
	T* Detach()
	{
		T* pt = p;
		p = 0;
		return pt;
	}
	
	T *p;  //the pointer itself.
};


typedef BlueBasicPtr<IRoot> IRootPtr;

// A utility class used by BluePtr and BlueCastPtr. Do not use.
template <typename T>
class [[nodiscard]] BlueCastPtrImpl
{
	BlueCastPtrImpl( T* pointer )
		:m_pointer( pointer )
	{
	}

	template<typename R> friend BlueCastPtrImpl<R> BlueCastPtr( R* pointer );
	template<typename R> friend class BluePtr;

	T* m_pointer;
};

// Utility function used for copy constructing or assigning smart pointers
// with a dynamic cast (QueryInterface).
template <typename T>
BlueCastPtrImpl<T> BlueCastPtr( T* pointer )
{
	return BlueCastPtrImpl<T>( pointer );
}

template <typename T>
BlueCastPtrImpl<T> BlueCastPtr( const BlueBasicPtr<T>& pointer )
{
	return BlueCastPtr( pointer.p );
}

// A subclass that adds QueryInterface capability.  Cannot be used for the IRoot template,
// therefore, it is separate.
template <class T>
class BluePtr :
	public BlueBasicPtr<T>
{
	typedef BluePtr<T> Class;
	typedef BlueBasicPtr<T> Base;
public:

	BluePtr() :  Base()
	{
	}

	BluePtr( T* other ) : Base( other )
	{
	}

	BluePtr( BlueBasicPtr<T> &&other )
	{
		this->p = other.p;
		other.p = nullptr;
	}

	BluePtr( const BlueBasicPtr<T> &other ) : Base( other )
	{
	}

	// "Dynamic cast" overload, to use with BlueCastPtr function
	template<typename Other>
	BluePtr( const BlueCastPtrImpl<Other>& other )
	{
		if( other.m_pointer )
		{
			reinterpret_cast<IRoot*>( other.m_pointer )->QueryInterface( BlueInterfaceIID<T>(), (void**)&this->p, BEQI_SILENT );
		}
		else
		{
			this->p = nullptr;
		}
	}

	explicit BluePtr( const Be::Clsid& clsid )
	{
		CreateInstance( clsid );
	}

	explicit BluePtr( BlueScriptValue obj ) : Base()
	{
		*this = BlueCastPtr( BlueUnwrapObjectFromScriptValue( obj ) );
	}

	// Return a special subclass, so that people don't accidentally do Class->Unlock()
	_NoLock<T>* operator->() const
	{
		CCP_ASSERT( this->p );
		return reinterpret_cast<_NoLock<T>*>( this->p );
	}

	T* operator=( T* lp )
	{
		if (lp)
		{
			reinterpret_cast<IRoot*>( lp )->Lock();
		}
		IRoot *tmp = reinterpret_cast<IRoot*>( this->p );
		if( this->p )
		{
			this->p = 0;
			tmp->Unlock();
		}

		return this->p = lp;
	}

	//Copy assignment.  Required for robustness
	Class& operator=( const Class &other )
	{
		*this = other.p;
		return *this;
	}
	
	// "Dynamic cast" overload, to use with BlueCastPtr function
	template <typename Other>
	Class& operator=( BlueCastPtrImpl<Other> obj ) 
	{
		IRoot* tmp = reinterpret_cast<IRoot*>( this->p );
		if( obj.m_pointer )
		{
			reinterpret_cast<IRoot*>( obj.m_pointer )->QueryInterface( BlueInterfaceIID<T>(), (void**)&this->p, BEQI_SILENT );
		}
		else
		{
			this->p = nullptr;
		}
		if( tmp )
		{
			tmp->Unlock();
		}
		return *this;
	}

	//Convenience functions
	// Create an instance of a class with the given Clsid. This defaults to the
	// Clsid for the class the pointer is templated for. Use BLUE_CLASS when
	// defining the class to automatically set up the type trait to get this
	// default behavior.
	bool CreateInstance(const Be::Clsid& clsid = BlueClassTypeTraits<T>::Class() )
	{
		CCP_ASSERT(!this->p);
		return BlueCreateInstance(clsid, GetIRootIID(), (void**) &(*this));
	}
};

template <typename T>
BluePtr<T> CreateInstance()
{
	BluePtr<T> ptr;
	ptr.CreateInstance();
	return ptr;
}


BLUE_DECLARE_INTERFACE_EXPORT(IWeakObject);	//an object that can be weakrefed

class BlueWeakRefBase : public IWeakRef
{
	typedef BlueWeakRefBase Class;

public:
	BlueWeakRefBase() : m_pointer( nullptr )
	{
	}

	~BlueWeakRefBase()
	{
		Assign( nullptr );
	}

	//Copy constructor
	BlueWeakRefBase(const Class &other) : m_pointer( nullptr )
	{
		Assign( other.m_pointer );
	}

	BlueWeakRefBase( IRoot* r ) : m_pointer( nullptr )
	{
		Assign( r );
	}

	//Assignment
	Class &operator=( const Class &other )
	{
		Assign( other.m_pointer );
		return *this;
	}

	Class &operator=( IRoot* other )
	{
		Assign( other );
		return *this;
	}

	bool operator <( const Class &other ) const
	{
		return m_pointer < other.m_pointer;
	}

	//are we valid?
	bool operator ! () const
	{
		return !m_pointer;
	}

	//Get the pointer, but be careful, lock it if needed.
	operator IRoot* () const
	{
		return m_pointer;
	}

	//for indirection, return a smart pointer so that we
	//are definitely locked during the call.
	BluePtr<IRoot> operator->() const
	{
		CCP_ASSERT( m_pointer );
		return BluePtr<IRoot>( m_pointer );
	}

	//IWeakRef interface:
	void WeakRefNotify( IWeakObject * )
	{
		m_pointer = 0;
	}

private:
	void Assign( IRoot *p )
	{
		if( m_pointer )
		{
			BluePtr<IWeakObject> obj( BlueCastPtr( m_pointer ) );
			CCP_ASSERT(obj);
			obj->WeakRefUnregister(this);
		}

		m_pointer = 0;

		if( p )
		{
			BluePtr<IWeakObject> obj( BlueCastPtr( p ) );
			if( obj )
			{
				obj->WeakRefRegister( this );
				m_pointer = p;
			}
		}
	}

protected:
	IRoot* m_pointer;
};

//A simple Weak pointer
template<class T> 
class BlueWeakRef : public BlueWeakRefBase
{
	typedef BlueWeakRef<T> Class;
	typedef BlueWeakRefBase Base;

public:
	BlueWeakRef() : Base()
	{
	}

	//Copy constructor
	BlueWeakRef(const Class &other) : Base( other )
	{
	}

	BlueWeakRef( T* r ) : Base( reinterpret_cast<IRoot*>( r ) )
	{
	}
		
	operator T* () const
	{
		return reinterpret_cast< T* >( m_pointer );
	}

	//for indirection, return a smart pointer so that we
	//are definitely locked during the call.
	BluePtr<T> operator->() const
	{
		CCP_ASSERT( m_pointer );
		return BluePtr<T>( reinterpret_cast<T*>( m_pointer ) );
	}
};


#endif
