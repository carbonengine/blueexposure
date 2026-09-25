// Copyright (c) 2026 CCP Games

#include "Copier.h"
#include "BluePyWrap.h"
#include "include/INotify.h"
#include "include/IInitialize.h"
#include "include/BlueListUtil.h"
#if BLUE_WITH_PYTHON
#include "include/PythonKlass.h"
#endif
#include "BlueVariable.h"
#include <CcpLog.h>

Copier::Copier()
:	m_override( nullptr ),
	m_overrideContext( nullptr ),
	m_postCopy( nullptr ),
	m_postCopyContext( nullptr ),
	mLevel( 0 )
{
}

void Copier::SetCopyOverrideCallback( CopyOverrideCallback copyOverride, void* context )
{
	m_override = copyOverride;
	m_overrideContext = context;
}

void Copier::SetPostCopyCallback( PostCopyCallback postCopy, void* context )
{
	m_postCopy = postCopy;
	m_postCopyContext = context;
}

//finally obsoleted.  Same as copy (copyto used to do a blind deep copy)
bool Copier::CloneTo(IRoot* source, IRoot** dest)
{
	return CopyTo(source, dest);
}


//The copier outer layer.  It handles the generation of a destination class
//if required, and optionally looks into maps to get previously created
//objects.
bool Copier::CopyTo(IRoot* source, IRoot** _dest)
{
	if( m_override )
	{
		switch( m_override( source, _dest, this, m_overrideContext ) )
		{
		case SUCCESS:
			return true;
		case FAILURE:
			return false;
		default:
			break;
		}
	}

	IRootPtr dest(*_dest);

	const Be::Clsid* sclsid = source->ClassType()->mClassId;

	if (!dest)
	{
		// We are expected to fill out the destination pointer.  First, if we
		// are cloning, see if we already created a destination matching this source.

		//Get the final iroot, necessary for comparisons
		source = BlueFinalIRoot(source);
			
		if (mPointers.get()) {
			tMapIter it = mPointers.get()->find(source);
			if (it != mPointers.get()->end()) {
				// Yes, we found it.  Just return it
				*_dest = (*it).second;
				(*_dest)->Lock();
				// We don't know what interface the caller expects. He has to
				// query it.
				return true;
			}
		}

		//Well, create an empty one, then.
		dest.CreateInstance(*sclsid);
		if (!dest)
			return false;

		// Good.  If we created it, put it into the pointer array.
		// must do this now, before recursing.
		if (!mPointers.get()) 
			//lazy generation of map
			mPointers = std::unique_ptr<tMap>(CCP_NEW( "Copier/mPointers" ) tMap);
		mPointers->insert( tPair(source, dest));

	}
	else
	{
		// assert same classes
		const Be::Clsid* destclsid = dest->ClassType()->mClassId;

		if (!sclsid->IsEqual(*destclsid))
		{
			CCP_LOGERR(
				"In CopyTo, 'source' and 'dest must be of same type. "
				"Source is %s.%s, dest is %s.%s",
				sclsid->GetModule(), sclsid->GetName(),
				destclsid->GetModule(), destclsid->GetName()
				);

			return false;
		}
	}
	
	// We now have a destination to copy to.
	mLevel++;
	bool result = CopyToInternal(source, dest);
	mLevel--;
	if (!result)
		return false;
	
	if (!*_dest)
		*_dest = dest.Detach();

	//after a single copying operation, clear the pointer array
	if (mLevel == 0 && mPointers.get())
		mPointers.get()->clear();

	if( m_postCopy )
	{
		m_postCopy( source, _dest, this, m_postCopyContext );
	}
	return true;
}


// Actually handles the copying of data.  dest must exist
bool Copier::CopyToInternal(IRoot* source, IRoot* dest) 
{
	INotifyPtr notify( BlueCastPtr( dest ) );	//for notifications of change to the dest
	IInitializePtr init( BlueCastPtr( dest ) );
	if( init )
	{
		// New style proper initialization - turn off notifications
		notify = (INotify*)0;
	}

	char *sourceFinal = (char*)BlueFinalIRoot(source);
	char *destFinal   = (char*)BlueFinalIRoot(dest);
	
	//Iterate over chained types in class
	const Be::ClassInfo* type;
	ssize_t offset = 0;
	for (type = dest->ClassType();
		 type;
		 offset += type->mOffsetToParent, type = type->mParentClassInfo)
	{
		const ssize_t interfaceOffs = type->mInterfaceTable->mOffset;
		char * const srcBase  = sourceFinal + offset - interfaceOffs;
		char * const destBase = destFinal   + offset - interfaceOffs;

		//Iterate over members
		const Be::VarEntry* memTable;
		for(memTable = type->mMemberTable; memTable->mType!=Be::INVALID; memTable++) {
			if (!(memTable->mEditFlags & Be::PERSIST))
				continue;
			
			Be::Var * const src = (Be::Var*)(srcBase  + memTable->mOffset);
			Be::Var * const dst = (Be::Var*)(destBase + memTable->mOffset);
			
			//This compare ensures that the on-modify callback is only made when we really
			//change things.
			if (memTable->mType != Be::CHARARRAY) {
				//hm, we want to avoid IROOT types too! but then we get callbacks.
				if (!memcmp(src, dst, memTable->mSize))
					continue;
			} else {
				//and compare arrays as strings
				if (!strncmp((const char *)src, (const char *)dst, memTable->mSize))
					continue;
			}
			
			BlueVariable* bv = GetBlueVariableFromVarType(memTable->mType);
			bool ok = bv->Copy(memTable, dst, src, this);
			if (!ok)
			{
				return false;
			}
			
			if (notify && !notify->OnModified(dst))
				return false;
		}
	}

	// Copy data that wasn't exposed as regular properties
	ICopierCustomAssignmentPtr srcprop( BlueCastPtr( source ) );
	if (srcprop && !srcprop->AssignTo(ICopierCustomAssignmentPtr( BlueCastPtr( dest ) ), this))
		return false;

#if BLUE_WITH_PYTHON
	//copy the deco of the class.
	BlueLockData *ld = BlueInternalHasLockData(source);
	if (ld && ld->mPythonKlass && !ld->mPythonKlass->CopyTo(source, dest))
		return false;
#endif

	if( init )
	{
		return init->Initialize();
	}

	return true;
}
