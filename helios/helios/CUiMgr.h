#pragma once
#include "stdafx.h"
#include "Helios_i.h"
#include "helios_itf.h"

using namespace ATL;

class ATL_NO_VTABLE CUiMgr :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUiMgr>,
	public ISupportErrorInfo,
	public IDispatchImpl<ICommandEvent, &__uuidof(ICommandEvent), &LIBID_HeliosLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IRibbonExtensibility, &__uuidof(IRibbonExtensibility), &LIBID_MSOffice, /* wMajor = */ 2, /* wMinor = */ 7>,
	public IUiMgr
{
public:
	BEGIN_COM_MAP(CUiMgr)
		COM_INTERFACE_ENTRY(IUiMgr)
		COM_INTERFACE_ENTRY2(IDispatch, ICommandEvent)
		COM_INTERFACE_ENTRY(ISupportErrorInfo)
		COM_INTERFACE_ENTRY(IRibbonExtensibility)
	END_COM_MAP()

private:
	//
	// ISupportsErrorInfo
	//
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

	//
	// IUiMgr
	//
	STDMETHOD_(void, CaptureExcelMainHwnd)();

	//
	// IRibbonExtensibility Methods
	//
	STDMETHOD(raw_GetCustomUI)(BSTR RibbonID, BSTR* RibbonXml);

	//
	// ICommandEvent
	//
	STDMETHOD(RbnOnLoad)(IDispatch* pRibbonUI);
	STDMETHOD(RbnOnBtnClick)(IDispatch* pDispCtrl);
	STDMETHOD(RbnOnChkBox)(IDispatch* pDispCtrl, BOOL bChecked);
	STDMETHOD_(IPictureDisp*, RbnLoadImage)(UINT image_id);
	STDMETHOD_(IPictureDisp*, RbnGetImage)(IDispatch* pDispCtrl);
	STDMETHOD_(LONG, RbnGetSize)(IDispatch* pDispCtrl);
	STDMETHOD_(BSTR, RbnGetDesc)(IDispatch* pDispCtrl);
	STDMETHOD_(BSTR, RbnGetLabel)(IDispatch* pDispCtrl);
	STDMETHOD_(BSTR, RbnGetKeyTip)(IDispatch* pDispCtrl);
	STDMETHOD_(BSTR, RbnGetScreenTip)(IDispatch* pDispCtrl);
	STDMETHOD_(BSTR, RbnGetSuperTip)(IDispatch* pDispCtrl);
	STDMETHOD_(VARIANT_BOOL, RbnGetEnabled)(IDispatch* pDispCtrl);
	STDMETHOD_(VARIANT_BOOL, RbnGetVisible)(IDispatch* pDispCtrl);
	STDMETHOD_(VARIANT_BOOL, RbnGetShowImage)(IDispatch* pDispCtrl);
	STDMETHOD_(VARIANT_BOOL, RbnGetShowLabel)(IDispatch* pDispCtrl);

	HRESULT GetResource(int nId, LPCTSTR lpType, LPVOID* ppvResourceData, DWORD* pdwSizeInBytes);
	BSTR GetXMLResource(int nId);
	static BOOL __stdcall EnumThreadHwnd(HWND hwnd, LPARAM lpParam);

private:
	CComQIPtr<IRibbonUI>	m_spRibbonUI;
	HWND					m_MainWnd;
};