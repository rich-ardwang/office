#pragma once
#include "stdafx.h"
using namespace ATL;

class CApp
{
public:
	CApp(LPDISPATCH Application);
	~CApp();

	inline CComPtr<_Application> get_Application() { return m_spApp; }

private:
	CComQIPtr< _Application >			m_spApp;
};