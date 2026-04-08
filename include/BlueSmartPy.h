// Copyright © 2026 CCP ehf.

/* 
	*************************************************************************

	BlueSmartVar.h

	Author:    Kristj�n Valur J�nsson
	Created:   Sep. 2003
	OS:        Win32
	Project:   Blue

	Description:   

		A smart pointer class for PyObject, which does automatic dereferecing.
		The pointer always owns the reference.  It can be made to give it away,
		however

	Dependencies:

		Python

	Examples:
	{
		//initialize with a new reference,use and automatically release
		BluePy p = PyString_FromString("boofaar");
		PyDict_SetItemString(dict, "karel", p);
	}

	{
		//initialize with a borrowed reference, and use
		BluePy p(PyList_GET_ITEM(list, 1), true);
		PyDict_SetItemString(dict, "karel", p);
	}

	{
		//initialize with a new string, and pass to someone who steals the reference
		BluePy p(PyString_FromStrin("muhaa"));
		PyTuple_SET_ITEM(tuple, 0, p.Stolen());
	}


	(c) CCP 2000

	*************************************************************************
*/

#ifndef _BLUESMARTPY_H_
#define _BLUESMARTPY_H_

class BluePyStr;

class BluePy {
public:

	//constructors.  We take our own reference
	BluePy() : o(0) {}

	//Initialize with our own reference. 
	BluePy(const BluePy &other) : o(other.o) {Py_XINCREF(o);}

	//Destructors:  We leave the reference
	~BluePy() {Py_XDECREF(o);}

	//intialize with a new reference, like the return value from a function.  Don't increment
	//unless it's a borrowed ref (like from PyList_GET_ITEM())
	explicit BluePy(PyObject *other, bool incref = false) : o(other) {
		if (incref)
			Py_XINCREF(o);
	}

	//assignment.  
	BluePy &operator=(const BluePy &other) {
		Py_XINCREF(other.o);
		Py_XDECREF(o);
		o = other.o;
		return *this;
	}
	
	//not operator
	bool operator !() const {return !o;}

	//Get the object pointer.  use this for calls that don't steal the reference.
	operator PyObject * () const { return o; }

	//The address of, used when initializing with an new reference
	PyObject ** operator &() {
		Py_XDECREF(o);
		o = 0;
		return &o;
	}
	
	//Detach the referenced object, give it away
	PyObject *Detach() {
		PyObject *res = o;
		o = 0;
		return res;
	}
	

	//And, to release our reference manually:
	void Release() {
		Py_XDECREF(o);
		o = 0;
	}
	
	//Get a new reference
	PyObject *NewRef() const {
		PyObject *res = o;
		Py_XINCREF(res);
		return res;
	}

	// Various common object operations 

	bool True() const {
		if (o)
			return PyObject_IsTrue(o) != 0;
		return false;
	}

	// Representation
	BluePyStr Repr() const;
	
	// We need to have this public, since sometimes we need the raw address of operator
public:
	PyObject *o;
};


// A specialization for tuples
class BluePyTuple : public BluePy
{
public:
	BluePyTuple(const BluePy &other) : BluePy(other) {}
	BluePyTuple() {}
	BluePyTuple(Py_ssize_t length) : BluePy(PyTuple_New(length), false){}
	
	//Assignment
	BluePyTuple &operator=(const BluePy &other) {
		BluePy::operator =(other);
		return *this;
	}

	bool Check() const {
		if (o)
			return PyTuple_Check((PyObject*)o) != 0;
		return false;
	}

	//special accessor methods, conforming to the regular api
	Py_ssize_t Size() const {
		return PyTuple_GET_SIZE((PyObject*)o);
	}
	BluePy Get(Py_ssize_t i) const {
		return BluePy(GET(i), true); //new reference
	}
	void Set(Py_ssize_t i, PyObject *val) const {
		Py_XINCREF(val);
		SET(i, val);
	}

	//fast special thingy, stels and borrows
	PyObject *GET(Py_ssize_t i) const {return PyTuple_GET_ITEM(o, i);}
	void SET(Py_ssize_t i, PyObject *val) const {PyTuple_SET_ITEM(o, i, val);}
};


// A specialization for lists
class BluePyList : public BluePy
{
public:
	BluePyList() {}
	BluePyList(const BluePy &other) : BluePy(other) {}
	BluePyList(Py_ssize_t length) : BluePy(PyList_New(length), false) {}
	
	//Assignment
	BluePyList &operator=(const BluePy &other) {
		BluePy::operator =(other);
		return *this;
	}

	bool Check() const {
		if (o)
			return PyList_Check((PyObject*)o) != 0;
		return false;
	}

	//special accessor methods, conforming to the regular api
	Py_ssize_t Size() const {
		return PyList_GET_SIZE((PyObject*)o);
	}

	BluePy Get(Py_ssize_t i) const {
		return BluePy(PyList_GetItem(o, i), true); //borrows
	}

	bool Set(Py_ssize_t i, PyObject *val) const {
		if (!val || 0 != PyList_SetItem(o, i, val)) //steals
			return false;
		Py_INCREF(val);
		return true;
	}

	bool Append(PyObject *val) const {
		return PyList_Append(o, val) == 0;
	}
	bool Insert(Py_ssize_t where, PyObject *val) const {
		return PyList_Insert(o, where, val) == 0;
	}

	BluePyTuple Tuple() const {
		return BluePy(PyList_AsTuple(o), false);
	}

	BluePyList Copy() const {
		Py_ssize_t s = Size();
		BluePyList res(s);
		for (Py_ssize_t i = 0; i<s; i++) {
			PyObject *e = PyList_GET_ITEM(o, i);
			PyList_SET_ITEM(res.o, i, e);
			Py_XINCREF(e);
		}
		return res;
	}	
};


// A simple wrapper for objects that obey the sequence protocol.
class BluePySeq : public BluePy
{
public:
	BluePySeq() {}
	BluePySeq(BluePy const &other) : BluePy(other) {}

	bool Check() const {
		if (o)
			return PySequence_Check((PyObject*)o) != 0;
		return false;
	}

	//Assignment
	BluePySeq &operator=(const BluePy &other) {
		BluePy::operator =(other);
		return *this;
	}


	//special accessor methods, conforming to the regular api
	Py_ssize_t Size() const {
		return PySequence_Size((PyObject*)o);
	}

	BluePy Get(Py_ssize_t i) const {
		return BluePy(PySequence_GetItem(o, i), false);
	}
	bool Set(Py_ssize_t i, PyObject *obj) const {
		return obj && PySequence_SetItem(o, i, obj)==0;
	}
	// here we can add the other sequence stuff in time

	BluePyTuple Tuple() const {
		return BluePy(PySequence_Tuple(o), false);
	}
	BluePyList List() const {
		return BluePy(PySequence_List(o), false);
	}
};


//Fast sequence.  This is used for speedy lookup
class BluePyFSeq : public BluePy
{
public:
	BluePyFSeq(PyObject *seq) : BluePy(PySequence_Fast(seq, "Expected a sequence")) {}

	//Assignment
	BluePyFSeq &operator=(const BluePyFSeq &other) {
		BluePy::operator =(other);
		return *this;
	}

	//special accessor methods, conforming to the regular api
	Py_ssize_t Size() const {
		return PySequence_Fast_GET_SIZE(o);
	}

	//Returns a borrowed reference
	PyObject *Peek(Py_ssize_t i) const {
		return PySequence_Fast_GET_ITEM(o, i);
	}

	//And here the more traditional one
	BluePy Get(Py_ssize_t i) const {
		return BluePy(Peek(i), true);
	}
};


// A specialization for dicts
class BluePyDict : public BluePy
{
public:
	BluePyDict() {}
	BluePyDict(const BluePy &other) : BluePy(other) {}
	BluePyDict(Py_ssize_t size) : BluePy(PyDict_New(), false) {}
	bool Check() const {
		if (o)
			return PyDict_Check((PyObject*)o) != 0;
		return false;
	}

	//Assignment
	BluePyDict &operator=(const BluePy &other) {
		BluePy::operator =(other);
		return *this;
	}

	//special accessor methods, conforming to the regular api
	Py_ssize_t Size() const {
		return PyDict_Size((PyObject*)o);
	}

	bool Has(PyObject *key) const {
		return PyDict_GetItem(o, key)!=0;
	}
	bool Has(const char *key) const {
		return PyDict_GetItemString(o, const_cast<char*>(key))!=0;
	}
	BluePy Get(PyObject *item) const {
		return BluePy(Peek(item), true); //return new reference
	}
	BluePy Get(const char *item) const {
		return BluePy(Peek(item), true); //return new reference
	}

	PyObject *Peek(PyObject *item) const {
		return PyDict_GetItem(o, item); //return a borrowed refernece
	}

	PyObject *Peek(const char *item) const {
		return PyDict_GetItemString(o, const_cast<char*>(item)); //return a borrowed refernece
	}


	//These have extra checks on them, so that they can be used in chained
	//operations that may fail
	bool Set(PyObject *key, PyObject *val) const {
		return key && val && PyDict_SetItem(o, key, val) == 0;
	}
	bool Set(const char *key, PyObject *val) const {
		return val && PyDict_SetItemString(o, const_cast<char*>(key), val) == 0;
	}
	bool Del(PyObject *key) const {
		return PyDict_DelItem(o, key) == 0;
	}
	bool Del(const char *key) const {
		return PyDict_DelItemString(o, const_cast<char*>(key)) == 0;
	}

	// misc dict specific stuff
	void Clear() const {
		PyDict_Clear(o);
	}
	BluePyDict Copy() const {
		return BluePy(PyDict_Copy(o), false);
	}
	BluePyList Items() const {
		return BluePy(PyDict_Items(o), false);
	}
	BluePyList Keys() const {
		return BluePy(PyDict_Keys(o), false);
	}
	BluePyList Values() const {
		return BluePy(PyDict_Values(o), false);
	}
	bool Merge(PyObject *other, bool override = false) const {
		return PyDict_Merge(o, other, override?1:0)==0;
	}
	bool Update(PyObject *other) const {return Merge(other, true);}
};


// A specialization for strings
class BluePyStr : public BluePy
{
public:
	BluePyStr() {}
	BluePyStr(const BluePy &other) : BluePy(other) {}

	// constructors, creating strings from char pointers.
	BluePyStr(const char *str) : BluePy(PyString_FromString(const_cast<char*>(str)), false) {}
	BluePyStr(size_t len, const char *str = 0) :
		BluePy(PyString_FromStringAndSize(const_cast<char*>(str), (len)), false)
	{}

	// Two static functions to create formatted strings.  Don't fit with the overloading
	// of constructor operands.
	static BluePyStr Format(const char *str, ...)
	{
		va_list args;
		va_start(args, str);
		BluePyStr r = FormatV(str, args);
		va_end(args);
		return r;
	}
	static BluePyStr FormatV(const char *format, va_list args) {
        va_list args1;
#ifdef _MSC_VER
		args1 = args;
#else
        va_copy(args1, args);
#endif
		size_t s = _vscprintf(format, args1);
#ifndef _MSC_VER
		va_end( args1 );
#endif
		BluePy str(PyString_FromStringAndSize(0, s));
		if (!str)
			return str;
		vsprintf_s(PyString_AS_STRING(str.o), s+1, format, args);
		return str;
	}

	
	bool Check() const {
		if (o)
			return PyString_Check(o) != 0;
		return false;
	}

	//Assignment
	BluePyStr &operator=(const BluePy &other) {
		BluePy::operator =(other);
		return *this;
	}

	Py_ssize_t Size() const {
		return PyString_Size(o);
	}
	
	//cool.  Now for some fun operators!
	BluePyStr operator+ (const BluePyStr &other) const {
		BluePyStr newStr(*this);
		PyString_Concat(&newStr.o, other.o);
		return newStr;
	}
	
	//concatenation operators
	BluePyStr &operator+= (const BluePyStr &other) {
		PyString_Concat(&o, other.o);
		return *this;
	}
	



	// to do "foo"+dude
	friend BluePyStr operator+(const char *left, const BluePyStr &right);
	
	//and to access the string
	const char *Str() const {return PyString_AsString(o);}
	const char *Str(Py_ssize_t &len) const {
		char *tmp;
		if (PyString_AsStringAndSize(o, &tmp, &len)) return 0;
		return tmp;
	}
	//to access no-null string (only a zero last.  raises exception otherwise)
	const char *CStr() const {
		char *tmp;
		if (PyString_AsStringAndSize(o, &tmp, 0)) return 0;
		return tmp;
	}
	
	//To format
	BluePyStr Format(PyObject *argtuple){
		return BluePy(PyString_Format(o, argtuple), false);
	}

	//Intern the string.  This puts it into some internal string table cache.  wired.
	void Intern() {
		PyString_InternInPlace(&o);
	}

	//Get a substring
	BluePyStr Slice(Py_ssize_t e) const {
		Py_ssize_t sz = Size();
		if (e < 0)
			e = sz + e;
		if (e < 0)
			e = 0;
		else if (e >= sz)
			e = sz-1;
		return BluePyStr(1, Str()+e);
	}

	BluePyStr Slice(Py_ssize_t start, Py_ssize_t end=0) const {
		Py_ssize_t sz = Size();
		if (start<0)
			start = sz+start;
		if (start < 0)
			start = 0;
		else if (start >= sz)
			start = sz-1;

		if (end <= 0)
			end = sz + end;
		if (end < start)
			end = start;
		else if (end > sz)
			end = sz;

		return BluePyStr(end-start, Str()+start);
	}
};

inline BluePyStr operator+(const char *left, const BluePyStr &right){
	return BluePyStr(left)+right;
}


// now we can define the Repr thing
inline BluePyStr BluePy::Repr() const {
	return BluePy(PyObject_Repr(o), false);
}
	

// A class for integers
// A specialization for strings
class BluePyInt : public BluePy
{
public:
	BluePyInt(int i) : BluePy(PyInt_FromLong(i)) {}
	BluePyInt(const BluePy &other) : BluePy(other) {}

	bool Check() const {
		if (o)
			return PyInt_Check(o) != 0;
		return false;
	}

	//Assignment
	BluePyInt &operator=(const BluePy &other) {
		BluePy::operator =(other);
		return *this;
	}

	int Int() const {
		return int(PyInt_AsLong(o));
	}

};

#endif