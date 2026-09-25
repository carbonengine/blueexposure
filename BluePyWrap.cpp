// Copyright (c) 2026 CCP Games

#if BLUE_WITH_PYTHON


#include "BluePyWrap.h"
#include "include/PythonKlass.h"
#include "include/IList.h"
#include "include/BlueListUtil.h"
#include "include/IBlueStructureList.h"
#include "include/BlueStructureList.h"
#include "include/IBlueDict.h"
#include "include/IBlueRtti.h"
#include "include/INotify.h"
#include "include/IPythonMethods.h"
#include "include/BlueScriptCallback.h"
#include "include/BlueSharedString.h"
#include <CcpLog.h>

#include "BlueClasses.h"
#include "BlueVariable.h"
#include "Copier.h"

#include <algorithm>
#include <frameobject.h>

#define CHECKOBJPTR \
	do {if (!CheckObjPtr()) return nullptr;} while (0)
 

BLUEIMPORT BluePythonObject* BlueWrapObjectForPython( IRoot* obj )
{
	return BlueWrapper::Create( obj );
}

BLUEIMPORT IRoot* BlueUnwrapObjectFromPython( PyObject* obj )
{
	return BlueWrapper::GetIRoot( obj );
}

BLUEIMPORT IRoot* BlueUnwrapObjectFromScriptValue( BlueScriptValue sv )
{
	return BlueWrapper::GetIRoot( sv );
}

//////////////////////////////////////////////////////////////////////
//
// Global variables and switches
//
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// The global list of all wrapped blue objects
//--------------------------------------------------------------------
BlueWrapper::Wrappers BlueWrapper::sWrappers( "BlueWrapper/sWrappers" );

//--------------------------------------------------------------------
// The wrapper cache
//--------------------------------------------------------------------
TrackableStdVector<BlueWrapper *> BlueWrapper::sWrapperCache( "BlueWrapper/sWrapperCache" );

//--------------------------------------------------------------------
// The global of all deco objects
//--------------------------------------------------------------------
BlueWrapper::DecoSet BlueWrapper::sDecos( "BlueWrapper/sDecos" );


//--------------------------------------------------------------------
// The global of all deco objects
//--------------------------------------------------------------------
#ifndef NDEBUG
BlueWrapper::TypeLocks BlueWrapper::sTypeLocks("BlueWrapper/sTypeLocks");
#endif


//////////////////////////////////////////////////////////////////////
//
// Static member functions
//
//////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------
// New and delete
//--------------------------------------------------------------------
void * BlueWrapper::operator new( size_t size, PyTypeObject* type )
{
	return PyObject_New( BlueWrapper, type );
}

void BlueWrapper::operator delete( void *ptr )
{
	PyObject_Del( ptr );
}

void BlueWrapper::operator delete(void *ptr, PyTypeObject* type)
{
	PyObject_Del( ptr );
}

BlueWrapper *BlueWrapper::GetWrapper( IRoot* obj, PyTypeObject* type )
{
	BlueWrapper *r;
	if (sWrapperCache.size()) {
		r = sWrapperCache.back();
		sWrapperCache.pop_back();
		PyObject_Init(r, type); // required when not doing new()
		r->Init(obj);
	} else
		r = new( type ) BlueWrapper(obj);
	r->PyPrepare();
	return r;
}

void BlueWrapper::ReturnWrapper(BlueWrapper *wrap) {
	wrap->PyUnprepare();
	if (sWrapperCache.size() < maxWrapperCache)
		sWrapperCache.push_back(wrap);
	else
		delete wrap;
}

void BlueWrapper::FlushWrappers()
{
	while (sWrapperCache.size()) {
		delete sWrapperCache.back();
		sWrapperCache.pop_back();
	}
}

BlueWrapper* BlueWrapper::Create(IRoot* object)
{
	// First, get lockdata, if we have it.
	BlueLockData* ld = BlueInternalHasLockData(object);
	if (ld && ld->mWrapper) {
		//yes, was already wrapped, just incref that
		Py_INCREF(ld->mWrapper);
		return ld->mWrapper;
	}

	// It didn't have a lock data, or didn't have a wrapper.  Got to try to insert in set.
	BlueWrapper tmpWrap(object);
	std::pair<WrapIt, bool> inserted = sWrappers.insert(&tmpWrap);
	
	BlueWrapper *wrap;
	if (inserted.second)
	{
		// Newly inserted.  Replace tmp with real
		PyTypeObject* type = object->ClassType()->mTypeObject;

		CCP_ASSERT( type );

		// Guarantee that the type is ready. Type objects are generally
		// finalized when the class is registered to the module but
		// templated lists/dicts need to be readied here. For a type
		// that is already finalized this is just a quick check.
		PyType_Ready( type );
		Py_INCREF( type );

		wrap = GetWrapper(object, type); //allocates from the wrapper cache

		// This is evil. The insert gives us back a const iterator as we're
		// not supposed to change the value after it's been inserted. This
		// used to be a regular iterator and this works as long as the value
		// doesn't change in a way that affects the ordering operator.
		// This changed to a const iterator in VS2010 - work around this for
		// now by casting away the const.
		BlueWrapper::WrapPointer& wp = const_cast<BlueWrapper::WrapPointer&>( *inserted.first );
		wp.wrap = wrap;
		wrap->Lock();
		wrap->IncType();
		wrap->TraceBack();

		//it may have a lockdata
		if (ld)
			ld->mWrapper = wrap;
	}
	else
	{
		// Already wrapped
		wrap = inserted.first->wrap;
		CCP_ASSERT(!ld); //we would have found it at the top of the function
		Py_INCREF(wrap);
	}
	
	return wrap;
}


//--------------------------------------------------------------------
// ReleaseDeco
// Called by Rot if it is deleting an object with a Deco.
//--------------------------------------------------------------------
void BlueWrapper::ReleaseDeco(PythonKlass **ppk)
{
	if (*ppk) {
		sDecos.erase(ppk);
		delete *ppk;
		*ppk = 0;
	}
}


//--------------------------------------------------------------------
// ReleaseAllDecos
// Called on shutdown.  It will modify the pointers in the Rot's BlueLockData
// and set them to 0.
//--------------------------------------------------------------------
void BlueWrapper::ReleaseAllDecos()
{
	while(sDecos.size()) {
		PythonKlass **ppKlass = *(sDecos.begin());
		PythonKlass *pKlass = *ppKlass;
		sDecos.erase(sDecos.begin());
		*ppKlass = 0; //erase the pointer in the BlueLockData
		delete pKlass;
	}
}


//--------------------------------------------------------------------
// GetIRoot
// Sees if this is a Blue object and returns the IRoot
//--------------------------------------------------------------------
IRoot *BlueWrapper::GetIRoot(PyObject *obj)
{
	PyTypeObject* type = obj->ob_type;
	while( type && (type != BePyTypePtr ) )
	{
		type = type->tp_base;
	}
	if( type == BePyTypePtr )
	{
		BlueWrapper *wrap = static_cast<BlueWrapper *>(obj);
		return wrap->Object();
	}
	return nullptr;
}


//--------------------------------------------------------------------
// GetWrapperList
// Creates a python list of all wrappers
//--------------------------------------------------------------------
PyObject* BlueWrapper::GetWrapperList()
{
	PyObject* list = PyList_New(sWrappers.size());
	if (!list)
	{
		return NULL;
	}

	int count = 0;
	for (WrapIt it = sWrappers.begin(); it != sWrappers.end(); ++it, count++)
	{
		PyList_SET_ITEM(list, count, it->wrap);
		Py_INCREF(it->wrap);
	}
	return list;
}


//--------------------------------------------------------------------
// Shudtown
// Dumps leaks and clears all data
//--------------------------------------------------------------------
void BlueWrapper::Shutdown()
{
	FlushWrappers();

	//Release all decos now
	ReleaseAllDecos();
	sWrappers.clear();

#ifndef NDEBUG
	sTypeLocks.clear();
#endif
}


//////////////////////////////////////////////////////////////////////
//
// Public member functions
//
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// Constructor
// Sets up the bare minimum needed to compare objects.  Full construction
// is in Prepare()
//--------------------------------------------------------------------
BlueWrapper::BlueWrapper(IRoot* obj) : 
	mWeakrefList( nullptr )
#ifndef NDEBUG
	, mTracebacks( "BlueWrapper/mTracebacks" )
#endif
{
	Init(obj);
}

void BlueWrapper::Init(IRoot* obj)
{
	mObj = BlueFinalIRoot(obj);
}


//--------------------------------------------------------------------
// Copy Constructor, used only very early
//--------------------------------------------------------------------
BlueWrapper::BlueWrapper(const BlueWrapper& x) :
	mWeakrefList( nullptr )
#ifndef NDEBUG
	, mTracebacks( "BlueWrapper/mTracebacks" )
#endif
{
	mObj = x.mObj;
}


//--------------------------------------------------------------------
// Lock
//--------------------------------------------------------------------
void BlueWrapper::Lock()
{
	mObj->Lock();
}


//--------------------------------------------------------------------
// Unlocker
//--------------------------------------------------------------------
void BlueWrapper::Unlock()
{
	mObj->Unlock();
}


//--------------------------------------------------------------------
// Compare function
//--------------------------------------------------------------------
bool BlueWrapper::operator < (const BlueWrapper& y) const
{
	return mObj < y.mObj;
}

#ifndef NDEBUG

void BlueWrapper::IncType(const Be::ClassInfo* obj)
{
	//use insert to give default value
	if (!obj)
		obj = Type();
	sTypeLocks.insert(TypeLocks::value_type(obj, 0)).first->second++;
}

void BlueWrapper::DecType(const Be::ClassInfo* obj)
{
	if (!obj)
		obj = Type();
	sTypeLocks[obj]--;
}

#else
void BlueWrapper::IncType(const Be::ClassInfo*){}
void BlueWrapper::DecType(const Be::ClassInfo*){}
#endif

bool BlueWrapper::mEnableTrace = false;


void BlueWrapper::PyPrepare()
{
	mWeakrefList = NULL;	
}


void BlueWrapper::PyUnprepare()
{
	if (mWeakrefList)
		PyObject_ClearWeakRefs(this);
}


IPythonNumeric *BlueWrapper::GetNumeric() const
{
	IPythonNumeric *result = dynamic_cast<IPythonNumeric*>( Object() );
	return result;
}


IList *BlueWrapper::GetSequence() const
{
	IList *result =dynamic_cast<IList*>( Object() );
	return result;
}


IPythonMethods *BlueWrapper::GetPyMethods() const
{
	IPythonMethods *result = dynamic_cast<IPythonMethods*>( Object() );
	return result;
}


IRoot *BlueWrapper::Object() const
{
		return mObj;
}


// The same, but additionally creates an error
IRoot *BlueWrapper::CheckObjPtr() const
{
	IRoot *result = Object();
	if( !result )
	{
		PyErr_SetString( PyExc_RuntimeError, "The Blue object is no longer valid, i.e. NULL" );
	}
	return result;
}


BlueLockData *BlueWrapper::LD() const
{
	CCP_ASSERT(mObj);
	return BlueInternalHasLockData(mObj);//get lockdata
}


//to find  a member variable, given the total offset to the final IRoot
Be::Var* BlueWrapper::MapMember(ssize_t fulloffset) const
{
	IRoot *obj = Object();
	return (Be::Var*)((uintptr_t)obj + fulloffset);
}



#ifndef NDEBUG

void BlueWrapper::TraceBack()
{
	if (!mEnableTrace)
		return;

	//Delete oldest ten tracebacks.
	if (mTracebacks.size() > 50)
	{
		Tracebacks::iterator start, end, i;
		start = mTracebacks.begin();
		end = start+10;
		for(i= start; i!= end; ++i)
			Py_DECREF(*i);
		mTracebacks.erase(start, end);
	}

	PyFrameObject *frame = PyEval_GetFrame();
	if (frame)
	{
		PyTraceBack_Here(frame);
		PyObject *cls, *val, *tb;
		PyErr_Fetch(&cls, &val, &tb);
		mTracebacks.push_back(tb);
		Py_XDECREF(cls);
		Py_XDECREF(val);
		Py_XDECREF(tb);
	}
}

#else

void BlueWrapper::TraceBack()
{
}

#endif


void BlueWrapper::NotSequence()
{
	PyErr_Format(
		PyExc_TypeError,
		"Object of type '%s' is not a list or dictionary", Type()->mClassId->GetName());
}


//Thunkers for standard python api
void BlueWrapper::PyDestroy_(PyObject* self)
{
	static_cast<_Class*>(self)->PyDestroy();
}
PyObject* BlueWrapper::PyGetAttr_(PyObject* self, char *name)
{
	return static_cast<_Class*>(self)->PyGetAttr(name);
}
int BlueWrapper::PySetAttr_(PyObject* self, char *name, PyObject* v)
{
	return static_cast<_Class*>(self)->PySetAttr(name, v);
}
PyObject* BlueWrapper::PyCompare_(PyObject* self, PyObject* other, int op)
{
	return static_cast<_Class*>(self)->PyCompare(other, op);
}
PyObject* BlueWrapper::PyRepr_(PyObject* self)
{
	return static_cast<_Class*>(self)->PyRepr();
}
Py_hash_t BlueWrapper::PyHash_(PyObject* self)
{
	return static_cast<_Class*>(self)->PyHash();
}
PyObject* BlueWrapper::PyStr_(PyObject* self)
{
	return static_cast<_Class*>(self)->PyStr();
}



//Standard python API implementation
// This is called when we lose the last Python reference. The wrapper
// is torn down and returned to the cache and the Blue object unlocked.
// The Blue object is not necessarily being destroyed - there may be
// other Blue references.
void BlueWrapper::PyDestroy()
{
	// Call the destroy handler for the Python methods
	IPythonMethods *methods = GetPyMethods();
	if (methods)
		methods->Destroy();

#ifndef NDEBUG
	for (unsigned i = 0; i < mTracebacks.size(); i++)
		Py_XDECREF(mTracebacks[i]);
	mTracebacks.clear();
#endif

	BlueLockData *ld = LD();
	if (ld)
		ld->mWrapper = 0; //clear weak reference

	sWrappers.erase(this);
	DecType();
	Unlock();
	ReturnWrapper(this); //return wrapper to the wrapper cache	
}

// This is called when the Blue object is about to die. This shouldn't
// really happen - the fact that it has a wrapper indicates that there
// are still Python references to it. Still need to investigate if
// there are cases where a wrapper exists without a Python reference.
void BlueWrapper::Destroy()
{
	if( Py_REFCNT( this ) > 0 )
	{
		CCP_LOGERR( "BlueWrapper::Destroy called on an object with %d Python references", Py_REFCNT( this ) );
#ifndef __clang_analyzer__
		char* const dummy = nullptr;
		*dummy = 0; // cppcheck-suppress nullPointer
#endif
	}

	// Return wrapper to the cache
	ReturnWrapper( this );
}


PyObject* BlueWrapper::PyGetAttr(const char* name)
{
	CHECKOBJPTR;
	bool parent = false;
	if (!strncmp(name, "_parent_", 8)) {
		//we want to skip the deco deliberately
		name += 8;
		parent = true;
	}
	
	//In the general case we always try a deco first
	BlueLockData *ld = LD();
	if (!parent && ld && ld->mPythonKlass) {
		bool handled = false;
		PyObject* retval = ld->mPythonKlass->GetAttr(name, &handled, this);
		if (handled)
			return retval;
	} 
		
	IPythonMethods *methods = GetPyMethods();
	if (methods)
	{
		// Then Python methods
		bool handled = false;
		PyObject* retval = methods->GetAttr(name, &handled);
		if (handled)
			return retval;
	}

	if (name[0] == '_')
	{
		// special underscore members
		if (strcmp(name, "__doc__") == 0)
		{
			return PyUnicode_FromString(Type()->mDescription ? Type()->mDescription : "");
		}
		else if (strcmp(name, "__typename__") == 0)
		{
			return PyUnicode_FromString(Type()->mClassId->GetName());
		}
		else if (strcmp(name, "__class__") == 0)
		{
			return PyObject_Type(this);
		}
		else if (strcmp(name, "__bluetype__") == 0) //better, gives full name
		{
			return PyUnicode_FromFormat("%s.%s", Type()->mClassId->GetModule(), Type()->mClassId->GetName());
		}
		else if (strcmp(name, "__klass__") == 0)
		{
			if (ld && ld->mPythonKlass)
				return ld->mPythonKlass->GetKlass();
		}
		else if (strcmp(name, "__dict__") == 0)
		{
			//used by python's builtin dir() command
			const char* defaults[] =
			{
#ifndef NDEBUG
				"__dotrace__", // Py_None
#endif
				"__doc__", // Py_Unicode
				"__typename__", // Py_Unicode
				"__bluetype__", // Py_Unicode
				"__iroot__" // Py_Capsule
			};
			int n = sizeof defaults / sizeof defaults[0];
			PyObject* dict = PyDict_New();
			for (int i = 0; i<n; i++)
			{
				if ( PyDict_SetItemString( dict, defaults[i], Py_None ) )
				{
					Py_XDECREF(dict);
					return nullptr;
				}
				Py_INCREF( Py_None );
			}
			if (ld && ld->mPythonKlass)
			{
				if( PyDict_SetItemString( dict, "__klass__", ld->mPythonKlass->GetKlass() ) )
				{
					Py_XDECREF( dict );
					return nullptr;
				}
			}

			//now loop over all variables
			for(const Be::ClassInfo* type = Type(); type; type = type->mParentClassInfo)
				for (const Be::VarEntry* entry = type->mMemberTable; entry->mName; entry++) {
					if (entry->mEditFlags & Be::HIDDEN)
						continue;
					if ( PyDict_SetItemString( dict, entry->mName, Py_None ) )
					{
						Py_XDECREF( dict );
						return nullptr;
					}
					Py_INCREF( Py_None );
				}
			return dict;
		}
		else if (strcmp(name, "__iroot__") == 0)
		{
			return PyCapsule_New(Object(), "__iroot__", nullptr);
		}

#ifndef NDEBUG
		else if (strcmp(name, "__dotrace__") == 0)
		{
			PyObject* file = PySys_GetObject((char*)"stderr");

			for (unsigned i = 0; i < mTracebacks.size(); i++)
			{
				PyTraceBack_Print(mTracebacks[i], file);
				PyFile_WriteString("\n\n", file);			
			}

			Py_INCREF(Py_None);
			return Py_None;
		}
#endif
	}

	// Is it a blue variable?  We search first in this smaller map for performance
	BlueRttiValue* val = BeClasses->GetRtti( Type() )->FindAttribute( name );
	if( val )
	{
		if (val->mType == BlueRttiValue::var)
		{
			//regular variable
			Be::Var* value = MapMember(val->mTotalOffset);
			return BlueConvertValueToPython(val->mVar, value);
		}
		else
		{
			//python property
			CCP_ASSERT(val->mType == BlueRttiValue::pyproperty);
			long flags = val->mPyProperty->mEditFlags;
			if( !(flags & Be::READ) )
			{
				return PyErr_Format(PyExc_AttributeError, "Python property '%s' is not gettable", name), nullptr;
			}
			BluePropertyGetterFunction f = val->mPyProperty->mGetProperty;
			return (*f)( this );
		}	
	}

	// is it a python method?
	val = BeClasses->GetRtti( Type() )->FindMethod( name );
	if( val )
	{
		CCP_ASSERT(val->mType == BlueRttiValue::pymethod);
		return PyCFunction_New(const_cast<PyMethodDef*>(val->mPyMethod), (PyObject *)this);
	}

	// No, well, last resort, if this is a deco class and has the getattr method, call that.
	if (!parent && ld && ld->mPythonKlass) {
		bool handled = false;
		//try __getattr__ handling on the deco
		PyObject* retval = ld->mPythonKlass->GetAttrFinal(this, name, &handled);
		if (handled)
			return retval;
	}

	val = BeClasses->GetRtti( Type() )->FindMethod( "__getattr__" );
	if( val )
	{
		CCP_ASSERT(val->mType == BlueRttiValue::pymethod);

		auto args = PyTuple_New( 1 );
		PyTuple_SET_ITEM( args, 0, PyUnicode_FromString( name ) );
		auto result = ( *val->mPyMethod->ml_meth )( (PyObject *)this, args );
		Py_DECREF( args );
		return result;
	}

	PyErr_SetString(PyExc_AttributeError, name);			
	return NULL;
}


int BlueWrapper::PySetAttr(const char* name, PyObject* v)
{
	/*
	The order of setattr/delattr on blue object is as follows:
	0.  If the attribute begins with _parent_, this is removed and any deco won't be
	    consulted.
	0.5 If the attribute begins with _nosetattr_, this is removed.  __setattr__ won't be invoked
		on the python deco.

	1.	If blue object implements IPythonMethods, then it's given the first chance
		to deal with it.

	3.	If the attribute is "__klass__", it means that Blue2PyWrapper is setting
		itself as part of deco magic. delattr will raise an error.

	4.	If the attribute exists on a deco instance, the value is set or the attribute
		is deleted (depending on getattr/delattr).

	5.	If the attribute does not exist on a deco instance AND not on the blue object
		itself, then the attribute is added to the deco (in case of setattr). delattr
		will raise an error.

	6.	If the attribute exists on the blue object, the value is set, or an error
		raised in case of delattr or read-only attribute. Notify on the blue object
		is sent, if appropriate.
	
	7.  If the attribute was set successfully, and the object has a deco, we send it the
	    OnSetAttr event, if present.
	*/
	bool parent = false;
	if (!strncmp(name, "_parent_", 8)) {
		parent = true;
		name += 8;
	}
	bool nosetattr = false;
	if (!strncmp(name, "_nosetattr_", 11)) {
		nosetattr = true;
		name += 11;
	}

	if( !CheckObjPtr() )
	{
		return 0;
	}

	bool handled = false;
 
	// CASE 1
	IPythonMethods *methods = GetPyMethods();
	if (methods)
	{
		bool ok = methods->SetAttr(name, v, &handled);
		if (handled && !ok)
			return -1;
	}

	// CASE 2
	
	// CASE 3
	if (!handled && strcmp(name,"__klass__") == 0)
	{
		BlueLockData *ld = LD();
		if (ld)
		{	
			PythonKlass *oldKlass = ld->mPythonKlass;
			if (v) {
				//Attach the instance of the Python metaclass Blue2PyWrapper
				//to this instance (by creating a PythonKlass object)
   				PythonKlass *newKlass = new PythonKlass(v);
				if (!newKlass || !newKlass->ok()) {
					delete newKlass;
					return -1;
				}
				ld->mPythonKlass  = newKlass;
				//if there wasn't anything here, we insert the new address of the
				//pointer
				if (!oldKlass)
					sDecos.insert(&ld->mPythonKlass);
				else {
					delete oldKlass;
				}
				return 0;
			} else {
				//removing the deco:
				if (oldKlass) {
					ld->mPythonKlass = 0;
					sDecos.erase(&ld->mPythonKlass);
					delete oldKlass;
					return 0;
				}
			}
		} else {
			if (v) {

			PyErr_SetString(
				PyExc_AttributeError, 
				"Can't set attribute __klass__ on a automatic blue object"
				);
			return -1;
			}
		}
	}

		
	// CASE 4
	PythonKlass* klass = NULL;
	BlueLockData *ld = LD();
	if (!handled && ld && ld->mPythonKlass && !parent) {
		klass = ld->mPythonKlass;
		int result = klass->SetAttr(&handled, this, name, v, false, nosetattr);
		if (result)
			return result; //error case
	}
		
	if (!handled) {
		const BlueRttiValue* rttiValue = BeClasses->GetRtti( Type() )->FindAttribute( name );

		// CASE 5
		if (!rttiValue)
		{
			if( klass )
			{
				//set this attribute, even if it didn't exist before
				int fail = klass->SetAttr(&handled, this, name, v, true, nosetattr);
				if (fail)
					return fail;
			}
			else if( v )
			{
				auto val = BeClasses->GetRtti( Type() )->FindMethod( "__setattr__" );
				if( val )
				{
					CCP_ASSERT(val->mType == BlueRttiValue::pymethod);

					auto args = PyTuple_New( 2 );
					PyTuple_SET_ITEM( args, 0, PyUnicode_FromString( name ) );
					PyTuple_SET_ITEM( args, 1, v );
					Py_INCREF( v );
					auto result = ( *val->mPyMethod->ml_meth )( (PyObject *)this, args );
					bool success = result != nullptr;
					Py_DECREF( args );
					Py_XDECREF( result );
					return success ? 0 : -1;
				}
			}
			else
			{
				auto val = BeClasses->GetRtti( Type() )->FindMethod( "__delattr__" );
				if( val )
				{
					CCP_ASSERT(val->mType == BlueRttiValue::pymethod);

					auto args = PyTuple_New( 1 );
					PyTuple_SET_ITEM( args, 0, PyUnicode_FromString( name ) );
					auto result = ( *val->mPyMethod->ml_meth )( (PyObject *)this, args );
					bool success = result != nullptr;
					Py_DECREF( args );
					Py_XDECREF( result );
					return success ? 0 : -1;
				}
			}
		}
	
		// CASE 6
		if (!handled) {
			if (!rttiValue)
			{
				PyErr_SetString(PyExc_AttributeError, name);
				return -1;
			}
			if (v == NULL)
			{
				PyErr_Format(PyExc_AttributeError, "Can't delete blue attribute %s", name);
				return -1;
			}

			if( rttiValue->mType == BlueRttiValue::var )
			{
				const Be::VarEntry *var = rttiValue->mVar;
				if ((var->mEditFlags & Be::WRITE) == 0)
				{
					PyErr_Format(PyExc_AttributeError, "Python property '%s' is read-only", name);
					return -1;
				}

				Be::Var* value = MapMember(rttiValue->mTotalOffset);
				if( !BlueConvertValueFromPython(var, value, v) )
				{
					return -1;
				}

				if (var->mEditFlags & Be::NOTIFY)
				{
					// Null out the pointer before we call this function. It may yield, which might
					// invalidate the map that this came from. 
					rttiValue = nullptr;

					INotifyPtr notify = BlueCastPtr( Object() );
					if (notify && !notify->OnModified(value))
					{
						// char* err;
						// todo: BeOS->FormatError(&err);
						PyErr_SetString(PyExc_AttributeError, name);
						// CCP_FREE( err );
						return -1;
					}		
				}
			}
			else
			{
				CCP_ASSERT(rttiValue->mType == BlueRttiValue::pyproperty);
				const Be::VarEntry *var = rttiValue->mPyProperty;
				BluePropertySetterFunction f = var->mSetProperty;
				if( !f )
				{
					PyErr_Format(PyExc_AttributeError, "Python property '%s' is not settable", name);
					return -1;
				}

				// Null out the pointer before we call this function. It may yield, which might
				// invalidate the map that this came from. 
				rttiValue = nullptr;

				PyObject *r = (*f)(this, v);
				Py_XDECREF(r);
				if( !r )
				{
					return -1;
				}
			}
		}
	}
	if (klass)
		//Send an OnSetAttr and OnDelAttr notification.
		return klass->OnSetAttr(this, name, v);
	return 0;
}

PyObject* BlueWrapper::PyCompare(PyObject* other, int op)
{
	IRoot *ptr = GetIRoot(other);
	if (!ptr)
	{
		PyErr_BadArgument();
		return nullptr;
	}
	switch(op)
	{
	case Py_EQ:
		if ( Object() == ptr ) {
			Py_RETURN_TRUE;
		} else {
			Py_RETURN_FALSE;
		}
		break;
	default:
		PyErr_Format(PyExc_NotImplementedError, "%d comparison operator not implement for object of type %.200s", op, Py_TYPE(other)->tp_name);
		return nullptr;
	}
}
		

PyObject* BlueWrapper::PyRepr()
{
	CHECKOBJPTR;

	//first, try to invoke a __repr__ dude, could be on the deco.
	if (PyObject_HasAttrString(this, "__repr__"))
		return PyObject_CallMethod(this, const_cast<char*>("__repr__"), const_cast<char*>("") );
		
	IPythonMethods *methods = GetPyMethods();
	if (methods) {
		bool handled = false;
		BluePy app(methods->Repr(&handled), false);		
		if (handled && !app)
			return 0;
		if (app)
			return app.Detach();
	}
	
	BluePyStr result = BluePyStr::Format("<BlueWrapper: %s.%s (%p)",
		Type()->mClassId->GetModule(), Type()->mClassId->GetName(), mObj);
	if (!result)
		return 0;
	
	BlueLockData *ld = LD();
	if (ld && ld->mPythonKlass){
		// Get the representation of the deco object
		BluePy app(ld->mPythonKlass->Repr(), false);
		if (!app) return 0;
		result += app;
	}
	if (result)
		result += BluePyStr(">");
	return result.Detach();
}


Py_hash_t BlueWrapper::PyHash()
{
	long long o = reinterpret_cast<long long>( Object() );  //note we are just hashing
	return (o & 0xffffffff) ^ (o >> 32);
}

PyObject* BlueWrapper::PyStr()
{
	CHECKOBJPTR;
	if (PyObject_HasAttrString(this, "__str__"))
		return PyObject_CallMethod(this, const_cast<char*>("__str__"), const_cast<char*>("") );

	//fallback to repr
	return PyRepr();
}


BLUEIMPORT PyObject* BlueConvertValueToPython(
	const Be::VarEntry* entry, 
	const Be::Var* value
	)
{
	PyObject* ret = NULL;
	Be::VARTYPE type = entry->mType;
	BlueVariable* bv = GetBlueVariableFromVarType(type);
	bv->ConvertToPython(entry, value, ret);
	return ret;
}

BLUEIMPORT bool BlueConvertValueFromPython(
	const Be::VarEntry* var, 
	Be::Var* value, 
	PyObject* v
	)
{
	BlueVariable* bv = GetBlueVariableFromVarType(var->mType);
	bool ok = bv->ConvertFromPython(var, value, v);

	if (!ok)
	{
		if (!PyErr_Occurred())
			PyErr_Format(PyExc_TypeError, "%s is type %d, got %s", var->mName, var->mType, v->ob_type->tp_name);
	}

	return ok;
}

BLUEIMPORT PyTypeObject* BePyTypePtr = nullptr;

PyTypeObject* BlueWrapper::InitPyType()
{
	if( BePyTypePtr )
	{
		return BePyTypePtr;
	}
	
	static PyTypeObject sPyType = 
		{
			PyVarObject_HEAD_INIT(&PyType_Type, 0)
			0,					/*					tp_name*/
			sizeof(BlueWrapper),/*					tp_basicsize*/
			0,					/*					tp_itemsize*/

			PyDestroy_,			/*destructor		tp_dealloc*/
			0,					/*printfunc			tp_print*/
			PyGetAttr_,			/*getattrfunc		tp_getattr*/
			PySetAttr_,			/*setattrfunc		tp_setattr*/
			0,			/*as_async			tp_as_async*/
			PyRepr_,			/*reprfunc			tp_repr*/
			0,					/*PyNumberMethods	*tp_as_number*/
			0,					/*PySequenceMethods *tp_as_sequence*/
			0,					/*PyMappingMethods	*tp_as_mapping*/
			PyHash_,			/*hashfunc			tp_hash*/
			0,					/*ternaryfunc		tp_call*/
			PyStr_,				/*reprfunc			tp_str*/
			0,					/*getattrofunc		tp_getattro*/
			0,					/*setattrofunc		tp_setattro*/
			0,					/*PyBufferProcs		*tp_as_buffer*/
			0,	/*long				tp_flags*/
			0,					/*char				*tp_doc*/
			0,					/*tp_traverse*/ //for GC
			0,					/*tp_clear*/	//for GC
			PyCompare_,					/*tp_richcompare*/
			BLUE_MEMBEROFFSET(BlueWrapper, mWeakrefList), /* tp_weaklistoffset */
			0,					/*tp_iter*/
			0,					/*tp_iternext*/
			0,					/*tp_methods*/
			0,					/*tp_members*/
			0,					/*tp_getset*/
			0,					/*tp_base*/
			0,					/*tp_dict*/
			0,					/*tp_descr_get*/
			0,					/*tp_descr_set*/
			0,					/*tp_dictoffset*/
			0,					/*tp_init*/
			0,					/*tp_alloc*/
			0,					/*tp_new*/
			0,					/*tp_free*/
			0,					/*tp_is_gc*/

		};

	std::string name = g_moduleName;
	name += ".";
	name += "BlueWrapper";

	sPyType.tp_name = CCP_STRDUP( "InitPyType/name", name.c_str() );
	sPyType.tp_base = &PyBaseObject_Type;

	BePyTypePtr = &sPyType;
	Py_INCREF(BePyTypePtr);

	// Note that we can not call PyType_Ready here - Python may not have been initialized.
	// That doesn't matter - any type referring to this base will eventually be readied
	// with PyType_Ready, and it in turn calls PyType_Ready for its base classes.

	return BePyTypePtr;
}


#define GETLIST(_retval) \
	IList* list = ( static_cast<_Class*>( self ) )->GetSequence();\
	if (!list) \
	{\
		( static_cast<_Class*>( self ) )->NotSequence();\
		return (_retval);\
	}

#define RETURNERR(_retval) \
	do{PyOS->PyError();return _retval;} while (0)



Py_ssize_t BlueWrapper::PyseqLength_(PyObject* self)
{
	GETLIST(-1);
	
	return list->GetSize();
}


class SliceList : public BlueList<IRoot>
{
public:

	ListInfo mInfo;

	void GetInfo(
		ListInfo* info
		)
	{
		*info = mInfo;
	}

};


PyObject* BlueWrapper::PyseqConcat_(PyObject* self, PyObject* listb)
{
	GETLIST(NULL);

	IList* list2 = ( static_cast<_Class*>( listb ) )->GetSequence();
	
	if (!list2)
	{
		( static_cast<_Class*>( self ) )->NotSequence();
		return NULL;
	}

	ssize_t size = list->GetSize();

	// create a copy of this list
	typedef RootRefLock<SliceList> OSliceList;
	OSliceList* tmp = new OSliceList;
	list->GetInfo(&tmp->mInfo);
	IListPtr other;
	other.Attach(tmp);


	for (int i = 0; i < size; i++)
	{
		IRoot* obj = list->GetAt(i);
		
		if (!obj)
			return NULL;

		if (!other->Insert(-1, obj))
			return NULL;
	}

	
	size = list2->GetSize();

	for (int i = 0; i < size; i++)
	{
		IRoot* obj = list2->GetAt(i);
		
		if (!obj)
			return NULL;

		if (!other->Insert(-1, obj))
			return NULL;
	}

	return Create(other);
}

PyObject* BlueWrapper::PyseqRepeat_(PyObject* self, Py_ssize_t n)
{
	GETLIST(NULL);

	if (n < 0)
		n = 0;
	
	ssize_t size = list->GetSize();

	// create a copy of this list
	typedef RootRefLock<SliceList> OSliceList;
	OSliceList* tmp = new OSliceList;
	list->GetInfo(&tmp->mInfo);
	IListPtr other;
	other.Attach(tmp);

	for (Py_ssize_t i = 0; i < n; i++)
	{
		for (ssize_t j = 0; j < size; j++)
		{
			IRoot* obj = list->GetAt(i);
			
			if (!obj)
				return NULL;

			if (!other->Insert(-1, obj))
				return NULL;
		}
	}
	return Create(other);
}


PyObject* BlueWrapper::PyseqGetItem_(PyObject* self, Py_ssize_t index)
{
	GETLIST(NULL);

    auto origIndex = index;

    // negative indices are treated as relative to the end of the sequence
    if ( index < 0 )
    {
        index = list->GetSize() + index;
    }

	if (index < 0 || index >= list->GetSize())
	{
		PyErr_Format(PyExc_IndexError, 
			"list index %zi out of range, size is %zi", origIndex, list->GetSize());
		return NULL;
	}

	IRoot* obj = list->GetAt(index);
	if (obj)
		return Create(obj);
	
	return nullptr;
}


PyObject* BlueWrapper::PyseqSlice_(PyObject* self, Py_ssize_t low, Py_ssize_t high)
{
	GETLIST(NULL);

	// create a copy of this list
	typedef RootRefLock<SliceList> OSliceList;
	OSliceList* tmp = new OSliceList;
	list->GetInfo(&tmp->mInfo);
	IListPtr other;
	other.Attach(tmp);

	PySlice_AdjustIndices( list->GetSize(), &low, &high, Py_ssize_t( 1 ) );

	// start copying
	for (Py_ssize_t i = low; i < high; i++)
	{
		IRoot* obj = list->GetAt(i);
		
		if (!obj)
			return NULL;

		if (!other->Insert(-1, obj))
			return NULL;
	}

	return Create(other);
}

int BlueWrapper::PySeqAssignSubscript_( PyObject* self, PyObject* key, PyObject* value )
{
	if ( PyIndex_Check( key ) ) {
		Py_ssize_t index;
		index = PyNumber_AsSsize_t( key, PyExc_IndexError );
		if ( index == -1 && PyErr_Occurred() ) {
			return -1;
		}
		return PyseqAssignItem_( self, index, value );
	} else if ( PySlice_Check( key ) ) {
		Py_ssize_t start, stop, step;

		if ( PySlice_Unpack( key, &start, &stop, &step ) ) {
			return -1;
		}

		if (step != 1) {
			PyErr_SetString( PyExc_NotImplementedError, "Supported for slices with a step size other than 1 is not implemented." );
			return -1;
		}

		return PyseqAssignSlice_( self, start, stop, value );
	}

	PyErr_Format( PyExc_TypeError, "list indices must be integers or slices, not %.200s", Py_TYPE(key)->tp_name );
	return -1;
}

PyObject* BlueWrapper::PySeqSubscript_( PyObject* self, PyObject* key )
{
	if ( PyIndex_Check( key ) ) {
		Py_ssize_t index;
		index = PyNumber_AsSsize_t( key, PyExc_IndexError );
		if ( index == -1 && PyErr_Occurred() ) {
			return nullptr;
		}
		return PyseqGetItem_( self, index );
	} else if ( PySlice_Check( key ) ) {
		Py_ssize_t start, stop, step;

		if ( PySlice_Unpack( key, &start, &stop, &step ) ) {
			return nullptr;
		}

		if (step != 1) {
			PyErr_SetString( PyExc_NotImplementedError, "Supported for slices with a step size other than 1 is not implemented." );
			return nullptr;
		}

		return PyseqSlice_( self, start, stop );
	}

	PyErr_Format( PyExc_TypeError, "list indices must be integers or slices, not %.200s", Py_TYPE(key)->tp_name );
	return nullptr;
}


int BlueWrapper::PyseqAssignItem_(PyObject* self, Py_ssize_t key, PyObject* value)
{
	GETLIST(-1);
	
	ListInfo info;
	list->GetInfo(&info);

	if ((info.mListOps & LIST_NOREMOVE) || (info.mListOps & LIST_NOINSERT))
	{
		PyErr_SetString( PyExc_RuntimeError, "No assignments allowed on this list.");
		return -1;
	}

	if (value)
	{
		IRoot* obj = BlueUnwrapObjectFromPython(value);
		
		if (!obj)
		{
			PyErr_SetString(PyExc_TypeError, "value ain't IRoot guy");
			return -1;
		}

		if (!list->Insert(key, obj))
		{
			return -1;
		}
		key += 1;
	}

	if (!list->Remove(key))
	{
		return -1;
	}

	return 0;
}

int BlueWrapper::PyseqAssignSlice_(PyObject* self, Py_ssize_t low, Py_ssize_t high, PyObject* object)
{
	GETLIST(-1);

	Py_ssize_t srcLen = object?PySequence_Size(object):0;
	if (srcLen<0)
		return -1;
	

	ListInfo info;
	list->GetInfo(&info);

	if ((info.mListOps & LIST_NOREMOVE) || (info.mListOps & LIST_NOINSERT))
	{
		PyErr_SetString( PyExc_RuntimeError, "No slicing allowed on this list.");
		return -1;
	}

	Py_ssize_t size = list->GetSize();

	if (low < 0)
		low = 0;
	else if (low > size)
		low = size;
	
	if (high < low)
		high = low;
	else if (high > size)
		high = size;
	
	// start removing
	Py_ssize_t i;

	if (low == 0 && high == size)
	{
		if (!list->Remove(-1))
			return -1;
	}
	else
	{
		for (i = low; i < high; i++)
		{
			if (!list->Remove(low))
				return -1;
		}
	}

	if (!object)
		return 0;

	// start adding
	for (i = 0; i < srcLen; i++)
	{
		PyObject *item = PySequence_GetItem(object, i);
		if (!item)
			return -1;
		IRoot* iroot = BlueUnwrapObjectFromPython(item);
		if (!iroot) {
			PyErr_SetString(PyExc_TypeError, "value ain't IRoot guy");
			Py_DECREF(item);
			return -1;
		}	
		if (!list->Insert(low + i, iroot)) {
			Py_DECREF(item);
			return -1;
		}
		Py_DECREF(item);
	}

	return 0;
}


int BlueWrapper::PyseqContains_(PyObject* self, PyObject* item)
{
	GETLIST(-1);

	IRoot* obj = BlueUnwrapObjectFromPython(item);
	
	if (!obj)
	{
		// It's not a BlueWrapper instance, so it can't be in the list.
		return 0;
	}

	return list->FindKey(obj) >= 0 ? 1 : 0;
}


//Number protocol functions

// binary and unary implement the numeric operations
PyObject* BlueWrapper::BinaryFunc(
	PYNUMERIC_OPS op, PyObject* other, const char* opstr)
{
	PyObject* retval = NULL;

	IPythonNumeric *numeric = GetNumeric();
	if (!numeric)
		return PyErr_Format(PyExc_TypeError, "left operand isn't IPythonNumeric"), nullptr;

	IRoot *otherRoot = GetIRoot(other);
	if (
		!otherRoot ||
		otherRoot->ClassType() != Type() ||
		!numeric->BinaryOp(op, otherRoot, &retval)
		)
	{
		PyErr_Format(PyExc_TypeError, "bad operand type(s) for %s", opstr);
	}

	return retval;
}


PyObject* BlueWrapper::UnaryFunc(
	PYNUMERIC_OPS op, const char* opstr)
{
	PyObject* retval = NULL;

	IPythonNumeric *numeric = GetNumeric();
	if (!numeric || !numeric->UnaryOp(op, &retval))
	{
		char tmp[128];
		sprintf_s(tmp, "bad operand type for unary %s", opstr);
		PyErr_SetString(PyExc_TypeError, tmp);
	}

	return retval;
}



#define IMPLEMENTBINARY(_fnpostfix, _op, _token)\
	PyObject* BlueWrapper::PyNumeric_##_fnpostfix ## _(PyObject* self, PyObject* other)\
	{\
		return static_cast<BlueWrapper*>(self)->BinaryFunc(_op, other, _token);\
	}
#define IMPLEMENTUNARY(_fnpostfix, _op, _token)\
	PyObject* BlueWrapper::PyNumeric_##_fnpostfix ## _(PyObject* self)\
	{\
		return static_cast<BlueWrapper*>(self)->UnaryFunc(_op, _token);\
	}

IMPLEMENTBINARY(Add, PYOP_ADD, "+")
IMPLEMENTBINARY(Sub, PYOP_SUB, "-")
IMPLEMENTBINARY(Mul, PYOP_MUL, "*")
IMPLEMENTBINARY(Div, PYOP_DIV, "/")

IMPLEMENTUNARY(Neg, PYOP_NEG, "-")
//IMPLEMENTUNARY(Pos, PYOP_POS, "+")
//IMPLEMENTUNARY(Int, PYOP_INT, "(int)")
//IMPLEMENTUNARY(Float, PYOP_FLOAT, "(float)")

int BlueWrapper::PyNumeric_NonZero_(PyObject *self)
{
	//This is called for the PyObject_IsTrue().  If the object is a list with 0 length,
	//return 0, otherwise, 1
	IList* list = ( static_cast<_Class*>( self ) )->GetSequence();
	if (list)
		return list->GetSize() > 0 ? 1 : 0;
	return 1;
}


int BlueWrapper::PyNumeric_Coerce_(PyObject** p1, PyObject** p2)
{
	PyObject* retval = NULL;
	IPythonNumeric* num = ((BlueWrapper*)*p1)->GetNumeric();

	if (num)
		num->Coercion(*p2, &retval);

	if (retval)
	{
		Py_INCREF(*p1);
		*p2 = retval;
		return 0;
	}

	return 1;
}

Py_ssize_t BlueWrapper::PyDictLength_( PyObject* self )
{
	return ( static_cast<BlueWrapper*>( self ) )->PyDictLength();
}

Py_ssize_t BlueWrapper::PyDictLength()
{
	IBlueDictPtr dict( BlueCastPtr( mObj ) );
	CCP_ASSERT( dict );

	return dict->GetLength();
}

PyObject* BlueWrapper::PyDictSubscript_( PyObject* self, PyObject* key )
{
	return ( static_cast<BlueWrapper*>( self ) )->PyDictSubscript( key );
}

PyObject* BlueWrapper::PyDictSubscript( PyObject* key )
{
	IBlueDictPtr dict( BlueCastPtr( mObj ) );
	CCP_ASSERT( dict );

	const char* keyString = PyUnicode_AsUTF8( key );
	if( keyString )
	{
		IRoot* value = dict->Subscript( keyString );
		if( value )
		{
			return Create( value );
		}
	}
	PyObject* reprObj = PyObject_Repr(key);
	PyErr_Format( PyExc_KeyError, "Key value %s not in dictionary", PyUnicode_AsUTF8(reprObj) );
	Py_XDECREF( reprObj );
	return NULL;
}

int BlueWrapper::PyDictAssignSubscript_( PyObject* self, PyObject* key, PyObject* value )
{
	return ( static_cast<BlueWrapper*>( self ) )->PyDictAssignSubscript( key, value );
}

int BlueWrapper::PyDictAssignSubscript( PyObject* key, PyObject* value )
{
	IBlueDictPtr dict( BlueCastPtr( mObj ) );
	CCP_ASSERT( dict );

	const char* keyString = PyUnicode_AsUTF8( key );

	IRoot* obj = NULL;
	if( value )
	{
		obj = BlueUnwrapObjectFromPython(value);
		if( !obj )
		{
			PyErr_SetString( PyExc_TypeError, "value is not a Blue object" );
			return -1;
		}
	}

	if( !dict->AssignSubscript( keyString, obj ) )
	{
		PyErr_SetString( PyExc_RuntimeError, "failed to assign to dict" );
		return -1;
	}
	
	return 0;
}

PyObject* BlueWrapper::PyseqDictGetItem_( PyObject* self, Py_ssize_t index )
{
	return ( static_cast<BlueWrapper*>( self ) )->PyseqDictGetItem( index );
}

PyObject* BlueWrapper::PyseqDictGetItem( Py_ssize_t index )
{
	IBlueDictPtr dict( BlueCastPtr( mObj ) );
	CCP_ASSERT( dict );

	const char* key = dict->GetKey( (size_t)index );
	if( key )
	{
		return PyUnicode_FromString( key );
	}

	return NULL;
}



Py_ssize_t BlueWrapper::PyStructureListLength_( PyObject* self )
{
	return ( static_cast<BlueWrapper*>( self ) )->PyStructureListLength();
}

Py_ssize_t BlueWrapper::PyStructureListLength()
{
	IBlueStructureListPtr structureList( BlueCastPtr( mObj ) );
	CCP_ASSERT( structureList );

	return structureList->GetSize();
}

PyObject* BlueWrapper::PyStructureListGetItem_( PyObject* self, Py_ssize_t index )
{
	return ( static_cast<BlueWrapper*>( self ) )->PyStructureListGetItem( index );
}

PyObject* BlueWrapper::PyStructureListGetItem( Py_ssize_t index )
{
	IBlueStructureListPtr structureList( BlueCastPtr( mObj ) );
	CCP_ASSERT( structureList );

	if( index < 0 || index >= (Py_ssize_t)structureList->GetSize() )
	{
		PyErr_Format(PyExc_IndexError, 
			"list index %zd out of range, size is %zu", index, structureList->GetSize());
		return nullptr;
	}

	uint8_t* item = (uint8_t*)structureList->GetAt( index );

	PyObject* returnValue = BlueStructureList_StructurePyObject( structureList, item );
	if( !returnValue )
	{
		PyErr_SetString( PyExc_RuntimeError, "unable to convert structure list to python" );
	}
	return returnValue;
}


int BlueWrapper::PyStructureListAssignItem_(PyObject* self, Py_ssize_t index, PyObject* value)
{
	return ( static_cast<BlueWrapper*>( self ) )->PyStructureListAssignItem( index, value );
}

int BlueWrapper::PyStructureListAssignItem(Py_ssize_t index, PyObject* value)
{
	IBlueStructureListPtr structureList( BlueCastPtr( mObj ) );
	CCP_ASSERT( structureList );

	if( index < 0 || index >= (Py_ssize_t)structureList->GetSize() )
	{
		PyErr_Format(PyExc_IndexError, 
			"list index %zi out of range, size is %zu", index, structureList->GetSize());
		return -1;
	}

	if( !value )
	{
		// this is a del operation
		structureList->Remove( index );
		return 0;
	}

	uint8_t* item = (uint8_t*)structureList->GetAt( index );

	try
	{
		BlueStructureList_PyObjectToStructure( structureList, value, item );
	}
	catch( std::exception& e )
	{
		PyErr_Format( PyExc_TypeError, "%s", e.what() );
		return -1;
	}
	structureList->ItemChanged( index );

	return 0;
}

void BlueWrapper::InitializeStandardTypeObject( PyTypeObject* pyType, const char* name )
{
	InitializeTypeObjectCommon( pyType, name );
	Py_INCREF( pyType );
}

void BlueWrapper::InitializeNumericTypeObject( PyTypeObject* pyType, const char* name )
{
	InitializeTypeObjectCommon( pyType, name );
	static PyNumberMethods s_numberMethods =
	{
			PyNumeric_Add_,		// binaryfunc nb_add;
			PyNumeric_Sub_,		// binaryfunc nb_subtract;
			PyNumeric_Mul_,		// binaryfunc nb_multiply;
			nullptr,			// binaryfunc nb_remainder;
			nullptr,			// binaryfunc nb_divmod;
			nullptr,			// ternaryfunc nb_power;
			PyNumeric_Neg_,		// unaryfunc nb_negative;
			nullptr,			// unaryfunc nb_positive;
			nullptr,			// unaryfunc nb_absolute;
			PyNumeric_NonZero_,	// inquiry nb_bool;
			nullptr,			// unaryfunc nb_invert;
			nullptr,			// binaryfunc nb_lshift;
			nullptr,			// binaryfunc nb_rshift;
			nullptr,			// binaryfunc nb_and;
			nullptr,			// binaryfunc nb_xor;
			nullptr,			// binaryfunc nb_or;
			nullptr,			// unaryfunc nb_int;
			nullptr,			// void* nb_reserved;
			nullptr,			// unaryfunc nb_float;
			nullptr,			// binaryfunc nb_inplace_add;
			nullptr,			// binaryfunc nb_inplace_subtract;
			nullptr,			// binaryfunc nb_inplace_multiply;
			nullptr,			// binaryfunc nb_inplace_remainder;
			nullptr,			// ternaryfunc nb_inplace_power;
			nullptr,			// binaryfunc nb_inplace_lshift;
			nullptr,			// binaryfunc nb_inplace_rshift;
			nullptr,			// binaryfunc nb_inplace_and
			nullptr,			// binaryfunc nb_inplace_xor
			nullptr,			// binaryfunc nb_inplace_or
			PyNumeric_Div_,		// binaryfunc nb_floor_divide;
			nullptr,			// binaryfunc nb_true_divide;
			nullptr,			// binaryfunc nb_inplace_floor_divide;
			nullptr,			// binaryfunc nb_inplace_true_divide;
			nullptr,			// unaryfunc nb_index;
			nullptr,			// binaryfunc nb_matrix_multiply;
			nullptr,			// binaryfunc nb_inplace_matrix_multiply;
	};

	pyType->tp_as_number = &s_numberMethods;
	Py_INCREF( pyType );
}

void BlueWrapper::InitializeDictTypeObject( PyTypeObject* pyType, const char* name )
{
	InitializeTypeObjectCommon( pyType, name );

	static PyMappingMethods s_dictMappingMethods =
	{
		PyDictLength_,			// lenfunc mp_length;
		PyDictSubscript_,		// binaryfunc mp_subscript;
		PyDictAssignSubscript_	// objobjargproc mp_ass_subscript;
	};

	static PySequenceMethods s_dictSequenceMethods =
	{
		nullptr,				// lenfunc sq_length;
		nullptr,				// binaryfunc sq_concat;
		nullptr,				// ssizeargfunc sq_repeat;
		PyseqDictGetItem_,		// ssizeargfunc sq_item;
		nullptr,				// [[unused]] void* sq_slice;
		nullptr,				// ssizeobjargproc sq_ass_item;
		nullptr,				// [[unused]] void* sq_ass_slice;
		nullptr,				// objobjproc sq_contains;
	};
	pyType->tp_as_mapping = &s_dictMappingMethods;
	pyType->tp_as_sequence = &s_dictSequenceMethods;
	Py_INCREF( pyType );
}

void BlueWrapper::InitializeListTypeObject( PyTypeObject* pyType, const char* name )
{
	InitializeTypeObjectCommon( pyType, name );

	static PyMappingMethods s_seqMappingMethods =
	{
		nullptr,				// lenfunc mp_length;
		PySeqSubscript_,		// binaryfunc mp_subscript;
		PySeqAssignSubscript_,	// objobjargproc mp_ass_subscript;
	};

	static PySequenceMethods s_sequenceMethods =
	{
		PyseqLength_,		// lenfunc sq_length;
		nullptr,			// binaryfunc sq_concat;
		nullptr,			// ssizeargfunc sq_repeat;
		PyseqGetItem_,		// ssizeargfunc sq_item;
		nullptr,			// [[unused]] void* sq_slice;
		PyseqAssignItem_,	// ssizeobjargproc sq_ass_item;
		nullptr,			// [[unused]] void* sq_ass_slice;
		PyseqContains_,		// objobjproc sq_contains;
	};

	pyType->tp_as_mapping = &s_seqMappingMethods;
	pyType->tp_as_sequence = &s_sequenceMethods;
	Py_INCREF( pyType );
}

void BlueWrapper::InitializeStructureListTypeObject( PyTypeObject* pyType, const char* name )
{
	InitializeTypeObjectCommon( pyType, name );

	static PySequenceMethods s_sequenceMethods =
	{
		PyStructureListLength_,		// lenfunc sq_length;
		nullptr,					// binaryfunc sq_concat;
		nullptr,					// ssizeargfunc sq_repeat;
		PyStructureListGetItem_,	// ssizeargfunc sq_item;
		nullptr,					// [[unused]] void* sq_slice;
		PyStructureListAssignItem_,	// ssizeobjargproc sq_ass_item;
		nullptr,					// [[unused]] void* sq_ass_slice;
		nullptr,					// objobjproc sq_contains;
	};

	pyType->tp_as_sequence = &s_sequenceMethods;
	Py_INCREF( pyType );
}

void BlueWrapper::InitializeTypeObjectCommon( PyTypeObject* pyType, const char* name )
{
	memset( pyType, 0, sizeof( PyTypeObject ) );
	pyType->ob_base.ob_base.ob_refcnt = 1;
	pyType->tp_base = InitPyType();
	pyType->tp_basicsize = sizeof( BlueWrapper );
	pyType->tp_name = CCP_STRDUP( "PyTypeObject/tp_name", name );
	pyType->tp_hash = PyHash_;
}

// Ensure the base PyTypeObject gets initialized
class PyTypeInitializer
{
public:
	PyTypeInitializer()
	{
		BlueWrapper::InitPyType();
	}
};

PyTypeInitializer pyTypeInitializer;


static PyObject* PyGetID( PyObject* module, PyObject* args )
{
	PyObject* o = NULL;
	if( !PyArg_ParseTuple( args, "O", &o ) )
	{
		return NULL;
	}

	IRoot* irootObject = BluePythonCast<IRoot*>( o );
	if( !irootObject )
	{
		PyErr_SetString( PyExc_BlueError, "Expected a blue object" );
		return NULL;
	}

	ssize_t ptrVal = (ssize_t)(irootObject);;
	return PyLong_FromSize_t( ptrVal );
}

MAP_FUNCTION( 
	"GetID", 
	PyGetID, 
	"GetID( o )\nGet's a unique ID (like the python id() function for Blue objects. The ID is the pointer address of the underlying IRoot instance.\n"
	":param obj: IRoot\n"
	":rtype: int" );

#endif
