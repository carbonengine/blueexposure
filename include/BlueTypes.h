// Copyright © 2026 CCP ehf.

#pragma once
#ifndef BlueTypes_h
#define BlueTypes_h

#include <CcpCore.h>
#include "BlueScriptValue.h"

#if !BLUE_WITH_PYTHON
#ifndef __ANDROID__
// TODO: ssize_t is not defined on WIN32 unless Python is included
typedef ptrdiff_t ssize_t;
#endif
#endif

struct IRoot;
struct IBlueRtti;



namespace Be
{
	typedef int64_t Time;	// Same as Win32 FILETIME

	template<typename T> struct Result;
}


template<typename T> bool BeIsSuccess( const Be::Result<T>& result )
{
	static_assert( sizeof( T ) == 0, "Missing specialization for BeIsSuccess" );
	CCP_ASSERT_M( false, "Missing specialization for BeIsSuccess" );
	return false;
}

template<typename T> const char* BeGetErrorMessage( const Be::Result<T>& result )
{
	static_assert( sizeof( T ) == 0, "Missing specialization for BeGetErrorMessage" );
	return "Missing specialization for BeGetErrorMessage";
}

#if BLUE_WITH_PYTHON
#define BLUE_DECLARE_GET_EXCEPTION( type ) template<> PyObject* BeGetException( const type& result );
#define BLUE_BEGIN_GET_EXCEPTION( type ) template<> PyObject* BeGetException( const type& result ) {
#define BLUE_BEGIN_GET_EXCEPTION_INLINE( type ) template<> inline PyObject* BeGetException( const type& result ) {
#define BLUE_END_GET_EXCEPTION() }

template<typename T> PyObject* BeGetException( const Be::Result<T>& result )
{
	static_assert( sizeof( T ) == 0, "Missing specialization for BeGetException" );
	CCP_ASSERT_M( false, "Missing specialization for BeGetException" );
	return nullptr;
}
#elif BLUE_NO_EXPOSURE
#define BLUE_DECLARE_GET_EXCEPTION( type ) template<> const char* BeGetException( const type& result );
#define BLUE_BEGIN_GET_EXCEPTION( type ) template<> const char* BeGetException( const type& result ) {
#define BLUE_BEGIN_GET_EXCEPTION_INLINE( type ) template<> inline const char* BeGetException( const type& result ) {
#define BLUE_END_GET_EXCEPTION() }

template<typename T> const char* BeGetException( const Be::Result<T>& result )
{
	static_assert( sizeof( T ) == 0, "Missing specialization for BeGetException" );
	CCP_ASSERT_M( false, "Missing specialization for BeGetException" );
	return nullptr;
}
#endif

namespace Be
{
	// --------------------------------------------------------------------------------------
	// Description:
	//   A helper object to wrap an optional exposed function argument for function that
	//   require special handling for ommited arguments, i.e. when initializing a missing
	//   parameter with a "0" default value is not enough.
	//   An Optional parameter carries a payload value and a boolean flag that is set to true
	//   if the parameter was actually passed or false if the parameter was omitted in the 
	//   script call.
	// --------------------------------------------------------------------------------------
	template <typename T>
	struct Optional
	{
		Optional()
			:m_value(),
			m_isAssigned( false )
		{
		}

		Optional( const T& value )
			:m_value( value ),
			m_isAssigned( true )
		{
		}

		Optional& operator=( const T& value )
		{
			m_value = value;
			m_isAssigned = true;
			return *this;
		}

		void SetAssigned( bool assigned )
		{
			m_isAssigned = assigned;
		}

		bool IsAssigned() const
		{
			return m_isAssigned;
		}

		const T& GetValue() const
		{
			return m_value;
		}

		T& GetValue()
		{
			return m_value;
		}

		operator const T&() const
		{
			return GetValue();
		}

		operator T&()
		{
			return GetValue();
		}
	protected:
		T m_value;
		bool m_isAssigned;
	};

	// --------------------------------------------------------------------------------------
	// Description:
	//   A specialized Be::Optional parameter that carries a specific default value. This
	//   class can only be used with POD types.
	// --------------------------------------------------------------------------------------
	template <typename T, T defaultValue>
	struct OptionalWithDefaultValue: public Be::Optional<T>
	{
		OptionalWithDefaultValue()
		{
			this->m_value = defaultValue;
		}

		OptionalWithDefaultValue( const T& value )
			:Be::Optional<T>( value )
		{
		}
	};

	struct BlueExposureFunctionSignature
	{
		const char* returnType;
		const char* argumentTypes[16];
		uint32_t argumentCount;
		uint32_t optionalCount;
	};

	// Class ID used at runtime - adds a hash of the name for
	// more efficient lookups.
	struct BLUEIMPORT Clsid
	{
	public:
		Clsid();
		Clsid( const char* modulename, const char* classname );

		bool InitFromString(const char* classid);
		bool IsEqual(const Clsid& other) const;
		bool operator <(const Clsid& other) const;
		operator bool() const;

		const char* GetModule() const;
		const char* GetName() const;
		unsigned int GetHash() const;

	private:
		const char* m_module;
		const char* m_name;
		unsigned int m_hash;
	};

	struct BLUEIMPORT IID
	{
	public:
		IID( const char* name );
		bool InitFromString(const char* classid);
		bool IsEqual(const IID& other) const;
		bool operator <(const IID& other) const;
		operator bool() const;

		const char* GetName() const;
		unsigned int GetHash() const;

	private:
		const char* m_name;
		unsigned int m_hash;
	};


	struct InterfaceEntry
	{
		const IID*	mIID;				// the IID to match
		ptrdiff_t	mOffset;			// offset
	};


	enum VARTYPE
	{
		// C types
		INVALID			=  0,		// no var / invalid
		LONG			=  1,		// 32 bit integer
		FLOAT			=  2,		// float (4 bytes)
		DOUBLE			=  3,		// double (8 bytes)
		BOOL			=  4,		// 1 byte bool
		IROOT			=  5,		// IRoot, or descendant, 4 bytes
		IROOTPTR		=  6,		// IRoot*, 4 bytes
		CHARARRAY		=  7,		// C string as char array
		CSTRING			=  8,		// C string malloc'ed and free'd
		INT64			=  9,		// 64bit integer
		PYOBJECT		= 10,		// Python object
		PYOBJECTPTR		= 11,		// Python object ptr
		REFERENCE		= 12,		// RotID reference
		WCSTRING        = 13,		// Wide malloced string
		WREFERENCE      = 14,		// exactly as WCSTRING, but with implied semantics
		FLOATARRAY		= 15,		// float[] array
		DOUBLEARRAY		= 16,		// double[] array
		INTARRAY		= 17,		// int[] array
		ROTREFERENCE	= 20,		// Special marker for streaming
		IROOTWEAKREF	= 21,		// BlueWeakRef to an IRoot descendant
		_PYPROPERTY		= 22,		// Python property function
		STDSTRING		= 23,		// C++ std::string
		STDWSTRING		= 24,		// C++ std::wstring
		BINARYBLOCK		= 25,		// A custom binary block

		BYTE			= 31,		// 8 bit integer
		SHORT			= 32,		// 16 bit integer

		SCRIPTCALLBACK	= 33,

		SHAREDSTRING	= 34,
		SHAREDSTRINGW   = 35,

		VARTYPE_MAX		= 35,

		VARTYPE_FORCELONG = 0xFFFFFFFF
	};	

	union Var
	{
		double			mDouble;
		int32_t			mLong;
		float			mFloat;
		bool			mBool;
		uint8_t			mByte;
		uint16_t		mShort;
		IRoot*			mIRootPtr;
		char*			mCharPtr;
		wchar_t*		mWCharPtr;
		int64_t			mInt64;
#if BLUE_WITH_PYTHON
		PyObject*		mPyObject;
#endif
	};


	// modification and editor flags
	enum EDITFLAGS
	{
		NONE			= 0x000,				// no particular flag set

		// modification and notification
		READ			= 0x001,				// value is accessible
		WRITE			= 0x002,				// value is modifiable
		READWRITE		= READ | WRITE,			// read/write value
		NOTIFY			= 0x004,
		HIDDEN			= 0x008,				// value is hidden
		MODMASK			= 0x00F,				// modification mask


		// serialization
		PERSIST			= 0x010,				// serialize automatically
		RPERSIST		= 0x020,				// like PERSIST but only unserialize (read only)
		SERMASK			= 0x0F0,				// serialization mask

		// editor support
		FLAGS			= 0x100,				// value  is a flag or mask
		ENUM			= 0x200,				// value is enumeration
		EDMASK			= 0xF00,				// editor mask

		// convenience
		PERSISTONLY		= HIDDEN | PERSIST,		// for hidden attributes

		// force enum to  long
		EDIT_FORCELONG	= 0xFFFFFFFF,
	};


	struct VarChooser
	{
		const char*		mKey;
		Var				mValue;
		const char*		mDescription;
	};

	struct VarEntry
	{
		// basic member type info
		const char*			mName;
		VARTYPE				mType;
		ptrdiff_t			mOffset;
		size_t				mSize;
		const IID*			mIID;
		const char*			mDescription;

		// editing support
		long				mEditFlags;			// EDITFLAGS enum
		const VarChooser*	mChooserTable;

		// properties
		BluePropertyGetterFunction		mGetProperty;
		BluePropertySetterFunction		mSetProperty;

		size_t GetFloatArraySize() const
		{
			if( mType == Be::FLOATARRAY )
			{
				return mSize / sizeof(float);
			}
			else
			{
				return 1;
			}
		}

		size_t GetDoubleArraySize() const
		{
			if( mType == Be::DOUBLEARRAY )
			{
				return mSize / sizeof(double);
			}
			else
			{
				return 1;
			}
		}

		size_t GetIntArraySize() const
		{
			if( mType == Be::INTARRAY )
			{
				return mSize / sizeof(int);
			}
			else
			{
				return 1;
			}
		}
	};

	// TODO: Abstract the method table better - right now we're copying the Python structure
	// to the lua setup.
	struct ClassInfo
	{
		const Clsid*			mClassId;
		const IID*				mIID;
		const char*				mDescription;

		const InterfaceEntry*	mInterfaceTable;
		const VarEntry*			mMemberTable;
		const BlueMethodDefinition*	mPyMethodTable;
		const ClassInfo*		mParentClassInfo;
		ptrdiff_t				mOffsetToParent;

		// Run-time type information - not really mutable, but initialized
		// on demand
		mutable IBlueRtti*		mRtti;

#if BLUE_WITH_PYTHON
		PyTypeObject*			mTypeObject;
#endif

		mutable CcpAtomic<uint32_t>	mLiveCount;
		mutable CcpAtomic<uint32_t>	mLockCount;

		std::map<std::string, BlueExposureFunctionSignature>* mFunctionSignatures;
	};


	typedef bool (CreateInstance)
		(const IID& riid, void** ppv);


	struct ClassRegistration
	{
		const ClassInfo*		mType;
		CreateInstance*			mCreateFn;
		unsigned int			mFlags;

		enum Flags
		{
			DISABLE_PYTHON_CONSTRUCTION = 1 << 0,
		};
	};

	enum LOADOBJECT_INIT_FLAG
	{
		LDOBJ_INITIALIZE = 0,
		LDOBJ_DONT_INITIALIZE = 1
	};
};

template <class T>
static double BeCast(const T value)
{
	CCP_ASSERT(sizeof (value) <= sizeof (double));

	// this can probable be changed into a fancy cast, but
	// the compiler will put memcpy there anyway...
	double d = 0.0;
	memcpy(&d, &value, sizeof value);

	return d;
}

#ifdef _MSC_VER
#define BLUE_NOVTABLE   __declspec(novtable)
#else
#define BLUE_NOVTABLE
#endif

template<typename T> struct BlueClassTypeTraits;
template<typename T> const Be::IID& BlueInterfaceIID();

//////////////////////////////////////////////////////////////////////
//
// Blue Root Interface
//
//////////////////////////////////////////////////////////////////////

// Declare an interface without the smart pointer typedef
// This is needed to declare IRoot so it can be used in further definitions.
#define BLUE_DECLARE_INTERFACE_NO_PTR(_interface)											\
	const Be::IID& Get##_interface##IID();													\
	template<> const Be::IID& BlueInterfaceIID< _interface >()

// Declare an interface without the smart pointer typedef
// This is needed to declare IRoot so it can be used in further definitions.
#define BLUE_DECLARE_INTERFACE_NO_PTR_EXPORT(_interface) \
	BLUEIMPORT const Be::IID& Get##_interface##IID(); \
	template<> BLUEIMPORT const Be::IID& BlueInterfaceIID<_interface>()

enum BLUEQIOPT
{
	BEQI_NONE		= 0x0,
	BEQI_SILENT		= 0x1,
};


enum BLUERTFLAGS
{
	BLUERT_AUTOVAR	= 0x1,
};

struct IRoot;
BLUE_DECLARE_INTERFACE_NO_PTR_EXPORT( IRoot );

struct BLUE_NOVTABLE IRoot
{
	virtual const Be::ClassInfo* ClassType() const = 0;

	virtual bool QueryInterface(
		const Be::IID& riid,
		void** ppv,
		BLUEQIOPT options = BEQI_NONE
		) = 0;

	virtual void Lock() = 0;

	virtual void Unlock() = 0;

	virtual long GetFlags() = 0;

	virtual int GetRefCount() const = 0;

	// Get the base pointer for this object. Since all interfaces inherit
	// from IRoot, this helps resolve multiple inheritance issues.
	virtual IRoot* GetRootObject() const = 0;

protected:
	// FinalDelete is used for delayed deletes of Blue objects - when
	// the refcount goes to 0 the object is not deleted right away but
	// added to a list - this list is then processed once per frame.
	friend class BlueClasses;
	virtual void FinalDelete() = 0;
};

//--------------------------------------------------------------------
// Support for weak references.
//--------------------------------------------------------------------

//Object that supports weak references should implement this interface
struct BLUE_NOVTABLE IWeakObject;			//fwd decl.

//A weakref must implement this interface, to be notified in when a
//refee goes away.
struct BLUE_NOVTABLE IWeakRef
{
	// The WearReferable target  calls this when it is going away
	// Note that it is permissable for the callee to create a new
	// reference to the object, thus resurrecting the object!
	// Note that in this case, the IWeakRef will have to consider
	// itself unregistered, and has to register itself again if it wishes
	// to create a new weak reference.
	virtual void WeakRefNotify(IWeakObject *ptr) = 0;
};

//An object that can have weak references must support this interface, so that
//weak references can register themselves at the object
struct BLUE_NOVTABLE IWeakObject : public IRoot
{
	//When a new reference is created.  Pass in the address of the weak pointer.
	//When the device goes away, it will clear it.
	//Should the IWeakObject object die, it will call the IWeakRef::WeakObjectDying callback.
	//Note that multiple registrations of the same "ref" are allowed, and it then
	//needs multiple unregistration.  Likewise, such an object will get multiple
	//WeakRefNotify calls when the target object dies.
	virtual void WeakRefRegister(IWeakRef *ref) = 0;
	virtual void WeakRefUnregister(IWeakRef *ref) = 0;
};

BLUEIMPORT const Be::IID& GetIWeakObjectIID();


// Forward declare a Blue interface
#define BLUE_DECLARE_INTERFACE( _interface ) \
	struct _interface; \
	BLUE_DECLARE_INTERFACE_NO_PTR( _interface ); \
	typedef BluePtr<struct _interface> _interface##Ptr

// Forward declare a Blue interface
#define BLUE_DECLARE_INTERFACE_EXPORT( _interface )     \
	struct _interface;                           \
	BLUE_DECLARE_INTERFACE_NO_PTR_EXPORT( _interface ); \
	typedef BluePtr<struct _interface> _interface##Ptr

// Define a Blue interface 
#define BLUE_INTERFACE( _interface ) \
	BLUE_DECLARE_INTERFACE( _interface ); \
	struct BLUE_NOVTABLE _interface

// Define a Blue interface 
#define BLUE_INTERFACE_EXPORT( _interface ) \
	BLUE_DECLARE_INTERFACE_EXPORT( _interface ); \
	struct BLUE_NOVTABLE _interface

// Forward declare a Blue class
#define BLUE_DECLARE( _classname ) \
class _classname; \
	const Be::Clsid& Get##_classname##Clsid();\
	BLUE_DECLARE_INTERFACE_NO_PTR( _classname ); \
	typedef BluePtr<_classname> _classname##Ptr

#define BLUE_DECLARE_LIST( U ) BLUE_DECLARE_VECTOR( U )
#define BLUE_DECLARE_LIST_RO( U ) BLUE_DECLARE_VECTOR_RO( U )

// Creates the safe BlueList version of your class
#define BLUE_DECLARE_VECTOR( U ) \
	typedef BlueList< U > U##Vector; \
	TYPEDEF_BLUECLASS( U##Vector )

// Read-only version of BlueList
#define BLUE_DECLARE_VECTOR_RO( U ) \
	typedef BlueList< U, LIST_READONLY > U##VectorRO; \
	TYPEDEF_BLUECLASS( U##VectorRO )

// A BlueList for interfaces - cannot create default instances
#define BLUE_DECLARE_IVECTOR( U ) \
	typedef BlueList< U > U##Vector; \
	TYPEDEF_BLUECLASS( U##Vector )

// A BlueDict
#define BLUE_DECLARE_DICT( U ) \
	typedef BlueDict<U> U##Dict; \
	TYPEDEF_BLUECLASS( U##Dict )

#define BLUE_DECLARE_STRUCTURE_LIST( U ) \
	typedef BlueStructureList<U> U##StructureList; \
	TYPEDEF_BLUECLASS( U##StructureList ); \
	const Be::Clsid& Get##U##StructureList##Clsid();\
	BLUE_DECLARE_INTERFACE_NO_PTR( U##StructureList ); \
	typedef BluePtr<U##StructureList> U##StructureList##Ptr

#include "BlueSmartPtr.h"



#endif
