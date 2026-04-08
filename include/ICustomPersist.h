// Copyright © 2026 CCP ehf.

#pragma once
#ifndef ICustomPersist_h
#define ICustomPersist_h

#include "BlueTypes.h"

BLUE_INTERFACE( ICustomPersist ) : public IRoot
{
	// Writing
	virtual void GetWriteBufferAndSize( const char* memberName, unsigned char** buffer, size_t* bufferSize ) = 0;
	virtual void ReleaseWriteBuffer( unsigned char* buffer ) = 0;
	// Reading
	virtual unsigned char* AllocateReadBuffer( const char* memberName, size_t bufferSize ) = 0;
	virtual void SetBufferAndSize( const char* memberName, unsigned char* buffer, size_t bufferSize ) = 0;
};

#endif // ICustomPersist_h