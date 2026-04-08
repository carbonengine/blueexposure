// Copyright © 2026 CCP ehf.

namespace Be
{
	struct ClassInfo;
	struct VarEntry;
}

struct BlueRttiValue
{
	enum {var, pymethod, pyproperty} mType;
	union {
		struct {							//var
			const Be::VarEntry* mVar;
			ssize_t				mXtraOffset;
			ssize_t				mTotalOffset;
		};
		const BlueMethodDefinition *mPyMethod;		//pyMethod
		const Be::VarEntry  *mPyProperty;	//pyProperty
	};
};

struct IBlueRtti
{
#if BLUE_WITH_PYTHON
	virtual PyObject* GetMethodsAsList() = 0;
#endif
	virtual BlueRttiValue* FindAttribute( const char* name ) = 0;
	virtual BlueRttiValue* FindMethod( const char* name ) = 0;
};


