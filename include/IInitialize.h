// Copyright © 2026 CCP ehf.

#pragma once
#ifndef IInitialize_h
#define IInitialize_h

BLUE_INTERFACE( IInitialize ) : public IRoot
{
	virtual bool Initialize() = 0;
};

#endif // IInitialize_h