// SumsAddin.h : Declaration of the CSumsAddin

#pragma once
#include "resource.h"       // main symbols
#include "Helios_i.h"
#include "CApp.h"
#include "CUiMgr.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

class ATL_NO_VTABLE CSumsAddin :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CSumsAddin, &CLSID_SumsAddin>,
	public ISupportErrorInfo,
	public ISumsAddin,
	public IDispatchImpl<_IDTExtensibility2, &__uuidof(_IDTExtensibility2), &LIBID_AddInDesignerObjects, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CSumsAddin() : m_pApp(nullptr)
	{
		s_pAddin = this;
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_SUMSADDIN)

	BEGIN_COM_MAP(CSumsAddin)
		COM_INTERFACE_ENTRY(ISumsAddin)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(_IDTExtensibility2)
		COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(IDispatch), m_pUnkUiMgr.p)
		COM_INTERFACE_ENTRY_AGGREGATE(__uuidof(IRibbonExtensibility), m_pUnkUiMgr.p)
		COM_INTERFACE_ENTRY_FUNC(IID_NULL, 0, __This)
	END_COM_MAP()

	//
	// InternalRelease only change m_dwRef, not call   
	// "delete this" even if m_dwRef decrease to zero.
	// Note: "delete this" is behavior of Release function.
	//
	DECLARE_PROTECT_FINAL_CONSTRUCT()
	DECLARE_GET_CONTROLLING_UNKNOWN()

	HRESULT FinalConstruct()
	{
		//
		// here will increase m_dwRef of CComAggObj because of we hold m_pUnkDockbarMgr.
		// Note: should not increase m_dwRef of outer-obj, otherwise we can not destroy 
		// outer-obj forever. so GetControllingUnknown()'s implementation can not increase
		// m_dwRef.
		//
		return CUiMgr::CreateInstance(GetControllingUnknown(), &m_pUnkUiMgr);
	}

	void FinalRelease()
	{
		// destroy aggregation obj
		m_pUnkUiMgr.Release();
		s_pAddin = nullptr;
	}

	inline static CSumsAddin* GetAddin()
	{
		return s_pAddin;
	}

	inline CApp* GetApp()
	{
		return m_pApp;
	}

public:
	inline static HRESULT __stdcall __This(void* pThis, REFIID riid, void** ppObj, DWORD dw)
	{
		*ppObj = pThis;
		return S_OK;
	}

private:
	//
	// ISupportsErrorInfo
	//
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//
	// _IDTExtensibility2 Methods
	//
	STDMETHOD(OnConnection)(LPDISPATCH Application, ext_ConnectMode ConnectMode, LPDISPATCH AddInInst, SAFEARRAY** custom);
	STDMETHOD(OnDisconnection)(ext_DisconnectMode RemoveMode, SAFEARRAY** custom);
	STDMETHOD(OnAddInsUpdate)(SAFEARRAY** custom);
	STDMETHOD(OnStartupComplete)(SAFEARRAY** custom);
	STDMETHOD(OnBeginShutdown)(SAFEARRAY** custom);

private:
	static CSumsAddin* s_pAddin;
	CApp* m_pApp;

	//
	// Note: for outer-obj, it can only hold "non-delegate IUnknow" to 
	// aggregated obj, otherwise outer-obj can not be destroyed forever.
	//
	CComPtr<IUnknown>	m_pUnkUiMgr;
};

OBJECT_ENTRY_AUTO(__uuidof(SumsAddin), CSumsAddin)
