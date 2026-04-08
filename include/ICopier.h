// Copyright © 2026 CCP ehf.

#pragma once
#ifndef ICopier_h
#define ICopier_h

#include "BlueTypes.h"

BLUE_INTERFACE_EXPORT( ICopier ) : public IRoot
{
	enum OverrideResult
	{
		SUCCESS,
		FAILURE,
		FALLBACK,
	};
	typedef OverrideResult (*CopyOverrideCallback)( IRoot* source, IRoot** dest, ICopier* copier, void* context );
	typedef void (*PostCopyCallback)( IRoot* source, IRoot** dest, ICopier* copier, void* context );

	virtual void SetCopyOverrideCallback( CopyOverrideCallback copyOverride, void* context ) = 0;
	virtual void SetPostCopyCallback( PostCopyCallback postCopy, void* context ) = 0;
	virtual bool CopyTo(IRoot* source, IRoot** dest) = 0;
};

// --------------------------------------------------------------------------------------
//  Description:
//    Provides the ability to extend the behavior of copying of Blue objects, beyond
//    the built in Be::VarTypes. This is used for copying of Dicts, Lists, etc. as well
//    as other information that is not intended to be persisted.
// --------------------------------------------------------------------------------------
BLUE_INTERFACE_EXPORT( ICopierCustomAssignment ) : public IRoot
{
	// Custom assignment function to a pre-created copy
	// 'other' is guaranteed to be of the same type as your class
	virtual bool AssignTo(
		ICopierCustomAssignment* other,
		ICopier* copier
		) = 0;
};

#endif
