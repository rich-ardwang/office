//
//  SumsAddin.cpp
//
//  Created by Colin on 2020-03-02.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#include "stdafx.h"
#include "SumsAddin.h"

CSumsAddin* CSumsAddin::s_pAddin = nullptr;

CSumsAddin::CSumsAddin() : m_pApp(nullptr)
{
	s_pAddin = this;
	log_info(helios_module, "Create CSumsAddin: this = 0x%x", this);
}

CSumsAddin::~CSumsAddin()
{
	log_info(helios_module, "Destroy CSumsAddin: this = 0x%x", this);
}

STDMETHODIMP CSumsAddin::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] = 
	{
		&IID_ISumsAddin
	};

	for (int i=0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CSumsAddin::OnConnection(LPDISPATCH Application, ext_ConnectMode ConnectMode, LPDISPATCH AddInInst, SAFEARRAY** custom)
{
	log_info(helios_module, "Enter CSumsAddin::OnConnection");
	InitModulePath();
    m_pApp = new CApp(Application);
    ATLASSERT(m_pApp);

	ATLASSERT(m_pUnkUiMgr);
	CComQIPtr<IUiMgr> spUiMgr(m_pUnkUiMgr);
	ATLASSERT(spUiMgr);
	spUiMgr->Initialize();

	log_info(helios_module, "Leave CSumsAddin::OnConnection");
    return S_OK;
}

STDMETHODIMP CSumsAddin::OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY** custom)
{
	log_info(helios_module, "Enter CSumsAddin::OnDisconnection");
    delete m_pApp;
    m_pApp = nullptr;

	//
	// Protect addin from exception: maybe exit delay about 5 secs
	// becasue file watch dog starts a thread named dummy_worker in logcxx module,
	// the stop condition of this thread is FileWatchDog::interrped = -1. in file
	// watch dog destruction, FileWatchDog::interrped will be set to -1.
	//
	stop_log_watch_dog();	
	log_info(helios_module, "Leave CSumsAddin::OnDisconnection");
    return S_OK;
}

STDMETHODIMP CSumsAddin::OnAddInsUpdate(SAFEARRAY** custom)
{
	log_info(helios_module, "Enter CSumsAddin::OnAddInsUpdate");


	log_info(helios_module, "Leave CSumsAddin::OnAddInsUpdate");
	return S_OK;
}

STDMETHODIMP CSumsAddin::OnStartupComplete(SAFEARRAY** custom)
{
	log_info(helios_module, "Enter CSumsAddin::OnStartupComplete");

	m_pDoc->Initialzie();

	log_info(helios_module, "Leave CSumsAddin::OnStartupComplete");
	return S_OK;
}

STDMETHODIMP CSumsAddin::OnBeginShutdown(SAFEARRAY** custom)
{
	log_info(helios_module, "Enter CSumsAddin::OnBeginShutdown");


	log_info(helios_module, "Leave CSumsAddin::OnBeginShutdown");
	return S_OK;
}

void CSumsAddin::InitModulePath()
{
	HMODULE hModule = _AtlBaseModule.GetModuleInstance();

	WCHAR szPath[MAX_PATH];
	if (!GetModuleFileNameW(hModule, szPath, MAX_PATH))
	{
		log_info(helios_module, "GetModuleFileName failed: %d", GetLastError());
		return;
	}
	PathRemoveFileSpecW(szPath);
	m_module_path = szPath;
	m_module_path += L"\\";
}
