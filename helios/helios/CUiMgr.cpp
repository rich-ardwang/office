#include "stdafx.h"
#include "CUiMgr.h"
#include "LoginDlg.h"

STDMETHODIMP CUiMgr::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* const arr[] =
	{
		&__uuidof(IUiMgr),
        &__uuidof(ICommandEvent)
	};

	for (int i = 0; i < sizeof(arr) / sizeof(arr[0]); i++)
	{
		if (InlineIsEqualGUID(*arr[i], riid))
			return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP_(void) CUiMgr::CaptureExcelMainHwnd()
{
    ::EnumThreadWindows(::GetCurrentThreadId(), EnumThreadHwnd, (LPARAM)&m_MainWnd);
}

STDMETHODIMP CUiMgr::raw_GetCustomUI(BSTR RibbonID, BSTR* RibbonXml)
{
    if (!RibbonXml)
        return E_POINTER;

    *RibbonXml = GetXMLResource(IDR_RIBBON);
    return *RibbonXml ? S_OK : E_OUTOFMEMORY;
}

STDMETHODIMP CUiMgr::RbnOnLoad(IDispatch* pRibbonUI)
{
    m_spRibbonUI = pRibbonUI;
    ATLASSERT(m_spRibbonUI);
    return S_OK;
}

STDMETHODIMP CUiMgr::RbnOnBtnClick(IDispatch* pDispCtrl)
{
    CComQIPtr<IRibbonControl> rbn_ctrl = pDispCtrl;
    ATLASSERT(rbn_ctrl);

    CComBSTR ctrl_id;
    HRESULT hr = rbn_ctrl->get_Id(&ctrl_id);
    if (ctrl_id == "LoginButton")
    {
        CLoginDlg dlg;
        if (IDOK == dlg.DoModal())
        {
            //todo: login to CDH
            ::MessageBox(m_MainWnd, L"login success.", L"MessageBox", MB_OK);
        }
    }
    else if (ctrl_id == "")
    {

    }
    return S_OK;
}

STDMETHODIMP CUiMgr::RbnOnChkBox(IDispatch* pDispCtrl, BOOL bChecked)
{
    return S_OK;
}

STDMETHODIMP_(IPictureDisp*) CUiMgr::RbnLoadImage(UINT image_id)
{
    HBITMAP hBitmap = (HBITMAP)::LoadImage(_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(image_id), 
                                IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_DEFAULTCOLOR /*| LR_LOADFROMFILE*/ );
    PICTDESC pictdesc = { 0 };
    pictdesc.cbSizeofstruct = sizeof(PICTDESC);
    pictdesc.picType = PICTYPE_BITMAP;
    pictdesc.bmp.hbitmap = hBitmap;
    pictdesc.bmp.hpal = NULL; // (HPALETTE)::GetStockObject(DEFAULT_PALETTE);

    IPictureDisp* pReturn = nullptr;
    ::OleCreatePictureIndirect(&pictdesc, IID_IPictureDisp, TRUE, (void**)&pReturn);
    return pReturn;
}

STDMETHODIMP_(IPictureDisp*) CUiMgr::RbnGetImage(IDispatch* pDispCtrl)
{
    return nullptr;
}

STDMETHODIMP_(LONG) CUiMgr::RbnGetSize(IDispatch* pDispCtrl)
{
    return 0;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetDesc(IDispatch* pDispCtrl)
{
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetLabel(IDispatch* pDispCtrl)
{
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetKeyTip(IDispatch* pDispCtrl)
{
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetScreenTip(IDispatch* pDispCtrl)
{
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetSuperTip(IDispatch* pDispCtrl)
{
    return nullptr;
}

STDMETHODIMP_(VARIANT_BOOL) CUiMgr::RbnGetEnabled(IDispatch* pDispCtrl)
{
    return VARIANT_TRUE;
}

STDMETHODIMP_(VARIANT_BOOL) CUiMgr::RbnGetVisible(IDispatch* pDispCtrl)
{
    return VARIANT_TRUE;
}

STDMETHODIMP_(VARIANT_BOOL) CUiMgr::RbnGetShowImage(IDispatch* pDispCtrl)
{
    return VARIANT_TRUE;
}

STDMETHODIMP_(VARIANT_BOOL) CUiMgr::RbnGetShowLabel(IDispatch* pDispCtrl)
{
    return VARIANT_TRUE;
}

HRESULT CUiMgr::GetResource(int nId, LPCTSTR lpType, LPVOID* ppResourceData, DWORD* pSizeInBytes)
{
    HMODULE hModule = _AtlBaseModule.GetModuleInstance();
    if (!hModule)
        return E_UNEXPECTED;

    HRSRC hRsrc = ::FindResource(hModule, MAKEINTRESOURCE(nId), lpType);
    if ( !hRsrc )
        return HRESULT_FROM_WIN32(GetLastError());

    HGLOBAL hGlobal = ::LoadResource(hModule, hRsrc);
    if ( !hGlobal )
        return HRESULT_FROM_WIN32(GetLastError());
    
    *ppResourceData = ::LockResource(hGlobal);
    *pSizeInBytes = ::SizeofResource(hModule, hRsrc);
    return S_OK;
}

BSTR CUiMgr::GetXMLResource(int nId)
{
    LPVOID pResourceData = nullptr; DWORD dwSizeInBytes = 0;
    HRESULT hr = GetResource(nId, TEXT("XML"), &pResourceData, &dwSizeInBytes);
    if ( FAILED(hr) )
        return nullptr;

    // Assumes that the data is not stored in Unicode.
    CComBSTR cbstr(dwSizeInBytes, reinterpret_cast<LPCSTR>(pResourceData));
    return cbstr.Detach();
}

BOOL __stdcall CUiMgr::EnumThreadHwnd(HWND hwnd, LPARAM lpParam)
{
    char class_name[256] = {0};
    ::GetClassNameA(hwnd, class_name, 256);
    if (0 == _stricmp(class_name, "XLMAIN"))
    {
        *(HWND*)lpParam = hwnd;
        return FALSE; // stop enum
    }
    return TRUE; // continue enum
}
