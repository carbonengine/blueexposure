// Copyright © 2026 CCP ehf.


#ifndef _BLUEPYWRAP_H_
#define _BLUEPYWRAP_H_

#if BLUE_WITH_PYTHON

#include "include/BlueTypes.h"
#include "include/BluePythonObject.h"
#include "include/IList.h"
#include "include/IPythonNumeric.h"

#include <vector>


//////////////////////////////////////////////////////////////////////
//
// Blue Python wrapper class
//
//////////////////////////////////////////////////////////////////////

class PythonKlass;
struct IPythonNumeric;
struct IPythonMethods;
struct BlueLockData;

class BlueWrapper :
	public BluePythonObject
{
	typedef BlueWrapper _Class;

protected:
	// Operators new and delete.  The object is allocated by Python
	void *operator new ( size_t size, PyTypeObject* type );

	void operator delete ( void *ptr );
	void operator delete ( void *ptr, PyTypeObject* type );

public:
	static BlueWrapper* Create(IRoot* object);

	// Get the IRoot of a wrapped object, or 0
	static IRoot *GetIRoot(PyObject *obj);

	// Get python type object of the BlueWrap
	static PyTypeObject* InitPyType();

	// Called by the ROT when an object is about to go away.  This gives
	// us a chance to release any deco on the object
	static void ReleaseDeco(PythonKlass **ld);

	// Release all decos that we have
	static void ReleaseAllDecos();

	// Get a python list of all wrapped objects
	static PyObject *GetWrapperList();

	// python is shutting down.  Dump leaked wrappers and
	// release the wrapper set.
	static void Shutdown();

	// Turn on or off tracebacks for wrapped objects
	static bool mEnableTrace;

	static void InitializeStandardTypeObject( PyTypeObject* pyType, const char* name );

	static void InitializeTypeObjectCommon( PyTypeObject* pyType, const char* name );

	static void InitializeNumericTypeObject( PyTypeObject* pyType, const char* name );
	static void InitializeDictTypeObject( PyTypeObject* pyType, const char* name );
	static void InitializeListTypeObject( PyTypeObject* pyType, const char* name );
	static void InitializeStructureListTypeObject( PyTypeObject* pyType, const char* name );

private:
	// Private methods
	BlueWrapper(IRoot* obj);
	BlueWrapper(const BlueWrapper&);
	BlueWrapper &operator=(BlueWrapper const &other); //no assignment
	void Init(IRoot *obj);

	//comparison, comparing the wrapped object and list index.  Used for sets.
	bool operator < (const BlueWrapper &) const;
	size_t Hash() const;

	// Fully set up the python part of an object that was just constructed
	void PyPrepare();
	void PyUnprepare(); //ditto, when cleaning up

	IRoot *Object() const;  //returns the fixed up object.
	IRoot *CheckObjPtr() const; // as Object(), but sets an error too.

	IPythonNumeric *GetNumeric() const; //returns IPythonNumeric interface for the object
	IList *GetSequence() const; //returns IList interface for the object
	IPythonMethods *GetPyMethods() const; //return IPythonMethods interface for object

	BlueLockData *LD() const;  //get lockdata

	void TraceBack(); // Register A python traceback for the wrapper.

	// used for bookkeeping
	void IncType(Be::ClassInfo const*type = 0);
	void DecType(Be::ClassInfo const*type = 0);

	// Lock and unlock the referenced object
	void Lock();
	void Unlock();

	//Get the classtype
	const Be::ClassInfo* Type() {return Object()->ClassType();}

	//map the variable given the full offset
	Be::Var* MapMember(ssize_t fulloffset) const;

public:
	// Called by ROT when the underlying blue object dies
	void Destroy();

private:
	// Support for weak references in python
	PyObject* mWeakrefList;  // facilitates weak references in python
	

private:
	// To wrap a blueobject:
	// We store pointers to wraps, dynamically allocated
	struct WrapPointer
	{
		WrapPointer(BlueWrapper *_wrap) : wrap(_wrap) {}
		bool operator == ( const WrapPointer& other ) const { return wrap->mObj == other.wrap->mObj; }
		BlueWrapper* wrap;
	};
	
	struct WrapPointerHasher
	{
		size_t operator()( const WrapPointer& o ) const
		{
			return std::hash<IRoot*>()( o.wrap->mObj );
		}
	};
	
	typedef TrackableStdHashSet<WrapPointer, WrapPointerHasher> Wrappers;
	typedef Wrappers::iterator WrapIt;
	static Wrappers sWrappers;

	//Wrapper Cache
	static BlueWrapper *GetWrapper(IRoot* obj, PyTypeObject* type );
	static void ReturnWrapper(BlueWrapper *w);
	static void FlushWrappers();
	static const size_t maxWrapperCache = 100;
	static TrackableStdVector<BlueWrapper *> sWrapperCache;

	// To keep track of all decos, in order to release them later.
	typedef TrackableStdHashSet<PythonKlass **> DecoSet;
	static DecoSet sDecos;

#ifndef NDEBUG
	// In debug mode, we keep count of TypeLocks and tracebacks
	typedef TrackableStdMap<const Be::ClassInfo*, int> TypeLocks;
	typedef TypeLocks::iterator TypeLocksIt;
	static TypeLocks sTypeLocks;

	//We can store several python tracebacks
	typedef TrackableStdVector<PyObject*> Tracebacks;
	Tracebacks mTracebacks;
#endif

		
private:
	// Standard Python API 
	static void PyDestroy_(PyObject* self);
	void PyDestroy();
	static PyObject* PyGetAttr_(PyObject* self, char *name);
	PyObject* PyGetAttr(const char* name);
	static int PySetAttr_(PyObject* self, char *name, PyObject* v);
	int PySetAttr(const char* name, PyObject* v);
	static int PyCompare_(PyObject* self, PyObject* other);
	int PyCompare(PyObject* other);
	static PyObject* PyRepr_(PyObject* self);
	PyObject* PyRepr();
	static long PyHash_(PyObject* self);
	long PyHash();
	static PyObject* PyStr_(PyObject* self);
	PyObject* PyStr();

	// Python sequence API (for lists)
	static Py_ssize_t PyseqLength_(PyObject* self);
	int PyseqLength();
	static PyObject* PyseqConcat_(PyObject* self, PyObject* listb);
	PyObject *PyseqConcat(PyObject* listb);
	static PyObject* PyseqRepeat_(PyObject* self, Py_ssize_t n);
	PyObject* PyseqRepeat(int n);
	static PyObject* PyseqGetItem_(PyObject* self, Py_ssize_t index);
	PyObject* PyseqGetItem(int index);
	static PyObject* PyseqSlice_(PyObject* self, Py_ssize_t low, Py_ssize_t high);
	PyObject* PyseqSlice(Py_ssize_t low, Py_ssize_t high);
	static int PyseqAssignItem_(PyObject* self, Py_ssize_t index, PyObject* object);
	int PyseqAssignItem(int index, PyObject* object);
	static int PyseqAssignSlice_(PyObject* self, Py_ssize_t low, Py_ssize_t high, PyObject* object);
	int PyseqAssignSlice(Py_ssize_t low, Py_ssize_t high, PyObject* object);
	static int PyseqContains_(PyObject* self, PyObject* item);
	int PyseqContains(PyObject* item);

	// Python sequence API (for dicts)
	static PyObject* PyseqDictGetItem_(PyObject* self, Py_ssize_t index);
	PyObject* PyseqDictGetItem(Py_ssize_t index);

	// Python mapping API (for dicts)
	static Py_ssize_t PyDictLength_( PyObject* self );
	Py_ssize_t PyDictLength();
	static PyObject* PyDictSubscript_( PyObject* self, PyObject* key );
	PyObject* PyDictSubscript( PyObject* key );
	static int PyDictAssignSubscript_( PyObject* self, PyObject* key, PyObject* value );
	int PyDictAssignSubscript( PyObject* key, PyObject* value );

	// Python sequence API (for structure lists)
	static Py_ssize_t PyStructureListLength_( PyObject* self );
	Py_ssize_t PyStructureListLength();
	static PyObject* PyStructureListGetItem_(PyObject* self, Py_ssize_t index);
	PyObject* PyStructureListGetItem(Py_ssize_t index);
	static int PyStructureListAssignItem_(PyObject* self, Py_ssize_t index, PyObject* value);
	int PyStructureListAssignItem(Py_ssize_t index, PyObject* value);

	// Python numeric API
#define DECLAREBINARY(_postfix) \
	static PyObject *PyNumeric_ ## _postfix ## _(PyObject *self, PyObject *other);
#define DECLAREUNARY(_postfix) \
	static PyObject *PyNumeric_ ## _postfix ## _(PyObject *self);

	DECLAREBINARY(Add)
	DECLAREBINARY(Sub)
	DECLAREBINARY(Mul)
	DECLAREBINARY(Div)
	DECLAREUNARY(Neg)

	static int PyNumeric_NonZero_(PyObject *self);
	static int PyNumeric_Coerce_(PyObject **p1, PyObject **p2);

	// helpers for numeric API
	PyObject* BinaryFunc( PYNUMERIC_OPS op, PyObject* other, const char* opstr);
	PyObject* UnaryFunc(PYNUMERIC_OPS op, const char* opstr);


	// helper guys
	void NotSequence();
};

#endif

#endif // _BLUEPYWRAP_H_
