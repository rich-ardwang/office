// SumsAddin.cpp : Implementation of CSumsAddin

#include "stdafx.h"
#include "SumsAddin.h"

// CSumsAddin
// 00024413-0000-0000-C000-000000000046  AppEvents

CSumsAddin* CSumsAddin::s_pAddin = nullptr;

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
    m_pApp = new CApp(Application);
    ATLASSERT(m_pApp);

	ATLASSERT(m_pUnkUiMgr);
	CComQIPtr<IUiMgr> spUiMgr(m_pUnkUiMgr);
	ATLASSERT(spUiMgr);
	spUiMgr->CaptureExcelMainHwnd();

    return S_OK;
}

STDMETHODIMP CSumsAddin::OnDisconnection(ext_DisconnectMode RemoveMode, SAFEARRAY** custom)
{
    delete m_pApp;
    m_pApp = nullptr;
    return S_OK;
}

STDMETHODIMP CSumsAddin::OnAddInsUpdate(SAFEARRAY** custom)
{
    return S_OK;
}

STDMETHODIMP CSumsAddin::OnStartupComplete(SAFEARRAY** custom)
{
    return S_OK;
}

STDMETHODIMP CSumsAddin::OnBeginShutdown(SAFEARRAY** custom)
{
    return S_OK;
}
