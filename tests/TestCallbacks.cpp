// Copyright © 2026 CCP ehf.

#include "StdAfx.h"
#include "TestCallbacks.h"

TestCallbacks::TestCallbacks( IRoot* lockobj )
{
}

const BlueScriptCallback& TestCallbacks::GetCallback() const
{
	return m_callback;
}

void TestCallbacks::SetCallback( const BlueScriptCallback& callback )
{
	m_callback = callback;
}

bool TestCallbacks::HasValidCallback() const
{
	return m_callback;
}

bool TestCallbacks::CallCallbackVoid()
{
	return m_callback.CallVoid();
}

std::pair<bool, int> TestCallbacks::CallCallbackInt()
{
	int result = 0xDeadBeef;
	auto status = m_callback.Call( result );
	return std::make_pair( bool( status ), result );
}

std::pair<bool, std::string> TestCallbacks::CallCallbackStringInt( int arg0 )
{
	std::string result = "0xDeadBeef";
	auto status = m_callback.Call( result, arg0 );
	return std::make_pair( bool( status ), result );
}

std::pair<bool, int> TestCallbacks::CallCallbackIntStringFloat( const std::string& arg0, float arg1 )
{
	int result = 0xDeadBeef;
	auto status = m_callback.Call( result, arg0, arg1 );
	return std::make_pair( bool( status ), result );
}
