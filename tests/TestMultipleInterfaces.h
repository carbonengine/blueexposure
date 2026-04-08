// Copyright © 2026 CCP ehf.

#pragma once
#ifndef TestMultipleInterfaces_h
#define TestMultipleInterfaces_h

#include "BlueExposure.h"

BLUE_INTERFACE( ITestInterfaceOne ) : public IRoot
{
	virtual void MethodOne() = 0;
};

BLUE_INTERFACE( ITestInterfaceTwo ) : public IRoot
{
	virtual void MethodTwo() = 0;
};

BLUE_CLASS( TestMultipleInterfaces ) :
	public ITestInterfaceOne,
	public ITestInterfaceTwo
{
public:
	EXPOSE_TO_BLUE();

	//////////////////////////////////////////////////////////////////////////
	// ITestInterfaceOne
	void MethodOne() override;

	//////////////////////////////////////////////////////////////////////////
	// ITestInterfaceTwo
	void MethodTwo() override;

};

TYPEDEF_BLUECLASS( TestMultipleInterfaces );

#endif // TestMultipleInterfaces_h