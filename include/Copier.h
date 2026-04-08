// Copyright © 2026 CCP ehf.

/* 
	*************************************************************************

	Copier.h

	Author:    Matthias Gudmundsson
	Created:   August 2001
	OS:        Win32
	Project:   Yep

	Description:   

		Blue object copy mechanism


	Dependencies:

		Blue

	(c) CCP 2000

	*************************************************************************
*/

#ifndef _COPIER_H_
#define _COPIER_H_

#include "BlueExposureMacros.h"
#include "ICopier.h"
#include "BlueUtil.h"
#include <memory>

BLUE_DECLARE( Copier );

BLUE_CLASS( Copier ) : public ICopier
{
public:
	EXPOSE_TO_BLUE();

	Copier();

	//Copy a structure, but doesn't preserve topology.  i.e. multiple
	//shared children will become separate instances
	bool CopyTo(IRoot *source, IRoot **dest) override;

	//Copy a structure preserving topology by not creating redundant
	//instances.
	bool CloneTo(IRoot *source, IRoot **dest);

	void SetCopyOverrideCallback( CopyOverrideCallback copyOverride, void* context ) override;
	void SetPostCopyCallback( PostCopyCallback postCopy, void* context ) override;
private:

	// Handle copying between existing classes
	bool CopyToInternal(IRoot* source, IRoot* dest);

	// Same, for Python Objects
	bool CopyPyObjectPtr(Be::Var &dst, Be::Var &src);

	CopyOverrideCallback m_override;
	void* m_overrideContext;
	PostCopyCallback m_postCopy;
	void* m_postCopyContext;
	int mLevel; //the recursion level
	typedef BlueStdMap<IRoot *, IRoot *> tMap;
	typedef tMap::const_iterator tMapIter;
	typedef std::pair<IRoot *, IRoot *> tPair;
	std::unique_ptr<tMap> mPointers; 
};

TYPEDEF_BLUECLASS(Copier);

#endif