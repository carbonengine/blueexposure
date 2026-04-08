// Copyright © 2026 CCP ehf.

#pragma once

#ifndef BlueMemberIterator_h
#define BlueMemberIterator_h

#include "BlueTypes.h"

// BlueMemberIterator iterates over members of a Blue class.
class BlueMemberIterator
{
public:	
	BlueMemberIterator( const Be::ClassInfo* clsInfo );
	BlueMemberIterator( IRoot *object = 0 );
	BlueMemberIterator(const BlueMemberIterator &other);

	bool Eof() const;
	bool Reset();

	const Be::ClassInfo* Type() const;
	const Be::VarEntry* Entry() const;

	// This is only valid when the iterator was constructed from an instance
	Be::Var* Var( IRoot* object = NULL ) const;

	bool Next(int n = 1); //moves to nth field from here field

	Be::Var &operator * () const;
	const Be::VarEntry *operator->() const;
	BlueMemberIterator &operator ++ ();
	BlueMemberIterator &operator = (const BlueMemberIterator &other);

	// Skip equal fields that are equal.  return numbers skipped.
	int SkipEquals(IRoot* def);

private:
	bool IsInteresting() const;

	bool NextParent();
	bool IsEqual (Be::Var const *other);

	const bool mPersist_only;
	const bool mRead;  //if persist_only, narrow direction.
	IRootPtr mObject;
	ssize_t mOffs;
	const Be::ClassInfo* mType;
	const Be::VarEntry* mEntry;
};


#endif
