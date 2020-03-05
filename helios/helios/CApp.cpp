#include "stdafx.h"
#include "CApp.h"

CApp::CApp(LPDISPATCH Application)
{
	m_spApp = Application;
}

CApp::~CApp()
{
	m_spApp = nullptr;
}
