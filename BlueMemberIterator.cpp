// Copyright © 2026 CCP ehf.

#include "include/BlueMemberIterator.h"
#include "include/BlueUtil.h"
#include "include/IList.h"
#include "include/IBlueDict.h"
#include "include/IBlueStructureList.h"
#include "include/BlueSharedString.h"
#include "BlueVariable.h"
#include <string>
#include <cmath>

BlueMemberIterator::BlueMemberIterator( IRoot *object ) : 
	mPersist_only(true), 
	mRead(false), 
	mObject(object)
{
	if( object )
	{
		mType = mObject->ClassType();
		Reset();
	}
	else 
	{
		mEntry = 0;
		mType = 0;
	}
}

BlueMemberIterator::BlueMemberIterator(  const Be::ClassInfo* clsInfo ) :
	mPersist_only( true ),
	mRead( false ),
	mObject( nullptr )
{
	mType = clsInfo;
	Reset();
}

BlueMemberIterator::BlueMemberIterator( const BlueMemberIterator &other ) : 
mPersist_only(other.mPersist_only),
	mRead(other.mRead)
{
	*this = other;
}



bool BlueMemberIterator::Eof() const
{
	return !mEntry;
}

//Reset.  if persist, point to the first persist member
bool BlueMemberIterator::Reset()
{
	mOffs = 0;
	mEntry = mType->mMemberTable;
	
	if( !mEntry->mName && !NextParent() )
	{
		return false;
	}
	
	if( !IsInteresting() )
	{
		return Next();
	}
	return true;
}

// Go to next member
bool BlueMemberIterator::Next(int n)
{
	if( Eof() )
	{
		return false;
	}
	
	for( int i = 0; i<n; i++ )
	{
		do {
			mEntry++;
			if( !mEntry->mName && !NextParent() )
			{
				return false;
			}
		} while( !IsInteresting() );
	}
	
	return true;
}


// Go to next non-empty parent, or signal EoF;
bool BlueMemberIterator::NextParent()
{
	//Move to the next parent
	do {
		const Be::ClassInfo* parentType = mType->mParentClassInfo;
		if( !parentType )
		{
			// Eof!
			mEntry = 0;
			return false;
		}

		mOffs += mType->mOffsetToParent;
		mType = parentType;
		mEntry = mType->mMemberTable;
	} while (!mEntry->mName);

	return true;
}


int BlueMemberIterator::SkipEquals(IRoot* def)
{
	if( !def )
	{
		return 0;
	}

	int skipcount = 0;
	while( !Eof() )
	{
		if( !IsEqual( Var( def ) ) )
		{
			break;
		}
		skipcount++;
		Next();
	}

	return skipcount;
}


bool BlueMemberIterator::IsEqual(const Be::Var *b)
{
	Be::Var const *a = Var();
	BlueVariable* bv = GetBlueVariableFromVarType(mEntry->mType);
	return bv->AreEqual(mEntry, a, b);
}


bool BlueMemberIterator::IsInteresting() const
{
	CCP_ASSERT( !Eof() );
	
	if( !mPersist_only )
	{
		return true;
	}

	if( mRead )
	{
		if( mEntry->mEditFlags & (Be::PERSIST|Be::RPERSIST) )
		{
			return true;
		}
	}
	else
	{ // write
		if( mEntry->mEditFlags & Be::PERSIST )
		{
			return true;
		}
	}

	return false;
}



const Be::VarEntry* BlueMemberIterator::Entry() const
{
	return mEntry;
}

Be::Var & BlueMemberIterator::operator*() const
{
	CCP_ASSERT(!Eof()); return *Var();
}

const Be::VarEntry * BlueMemberIterator::operator->() const
{
	CCP_ASSERT(!Eof()); return mEntry;
}

BlueMemberIterator & BlueMemberIterator::operator++()
{
	Next(1); return *this;
}

BlueMemberIterator & BlueMemberIterator::operator=( const BlueMemberIterator &other )
{
	mObject = other.mObject;
	mOffs = other.mOffs;
	mType = other.mType;
	mEntry = other.mEntry;
	return *this;
}

Be::Var* BlueMemberIterator::Var( IRoot* object /*= NULL*/ ) const
{
	if( !object && !mObject )
	{
		return nullptr;
	}

	return BLUEMAPMEMBEROFFSET(object ? object : mObject.p, mEntry, mType, mOffs);
}

const Be::ClassInfo* BlueMemberIterator::Type() const
{
	return mType;
}

