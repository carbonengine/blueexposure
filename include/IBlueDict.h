// Copyright © 2026 CCP ehf.

#pragma once
#ifndef IBlueDict_h
#define IBlueDict_h

#include "BlueTypes.h"

BLUE_DECLARE_INTERFACE_EXPORT( IBlueDict );

BLUE_INTERFACE_EXPORT( IBlueDict ) : public IRoot
{
	virtual size_t GetLength() const = 0;
	virtual IRoot* Subscript( const char* key ) const = 0;
	virtual bool AssignSubscript( const char* key, IRoot* value ) = 0;
	virtual const char* GetKey( size_t index ) = 0;
};

#endif // IBlueDict_h