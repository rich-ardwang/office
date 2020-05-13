#include "stdafx.h"
#include "resource.h"
#include "SumsAddin.h"
#include "CUiMgr.h"
#include "LoginDlg.h"
#include "ConfigDlg.h"
#include "lava_utils_api.h"


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

STDMETHODIMP_(void) CUiMgr::Initialize()
{
    CaptureExcelMainHwnd();
    m_MsgWnd.Initialize(0);
}

//
// This mothod should be called in UI thread
//
STDMETHODIMP_(void) CUiMgr::InvalidateControl(__lv_in BSTR ctrlId)
{
    if (m_spRibbonUI)
        m_spRibbonUI->raw_InvalidateControl(ctrlId);
    else
        log_error(helios_module, "IRibbonUI interface is empty.");
}

STDMETHODIMP_(void) CUiMgr::InvalidateControl(__lv_in UINT ctrlId)
{
    // convert ctrlId from UINT to BSTR, then call function above.
}

STDMETHODIMP_(void) CUiMgr::PostCustomMessage(__lv_in UINT Msg, __lv_in WPARAM wParam, __lv_in LPARAM lParam)
{
    m_MsgWnd.PostMessage(Msg, wParam, lParam);
}

STDMETHODIMP CUiMgr::raw_GetCustomUI(BSTR RibbonID, BSTR* RibbonXml)
{
    if (!RibbonXml)
        return E_POINTER;

    *RibbonXml = GetXmlResource(IDR_RIBBON);
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
    if (ctrl_id == _T("LoginButton"))
    {
        CLoginDlg dlg;
        dlg.DoModal();
        InvalidateControl(_T("LoginButton"));
    }
    else if (ctrl_id == _T("ConfigButton"))
    {
        CConfigDlg dlg;
        dlg.DoModal();
        InvalidateControl(_T("ConfigButton"));
    }
    else if (ctrl_id == _T("ManSendButton"))
    {
        CSumsAddin::GetAddin()->GetDoc()->OnManualSend();
    }
    return S_OK;
}

STDMETHODIMP CUiMgr::RbnOnChkBox(IDispatch* pDispCtrl, BOOL bChecked)
{
    return S_OK;
}

STDMETHODIMP_(IPictureDisp*) CUiMgr::RbnLoadImage(UINT image_id)
{
    HBITMAP hBitmap = LoadImageFromResource(image_id);
    PICTDESC pictdesc = { 0 };
    pictdesc.cbSizeofstruct = sizeof(PICTDESC);
    pictdesc.picType = PICTYPE_BITMAP;
    pictdesc.bmp.hbitmap = hBitmap;
    pictdesc.bmp.hpal = NULL;

    IPictureDisp* pReturn = nullptr;
    ::OleCreatePictureIndirect(&pictdesc, IID_IPictureDisp, TRUE, (void**)&pReturn);
    return pReturn;
}

STDMETHODIMP_(IPictureDisp*) CUiMgr::RbnGetImage(IDispatch* pDispCtrl)
{
    CComQIPtr<IRibbonControl> rbn_ctrl = pDispCtrl;
    ATLASSERT(rbn_ctrl);

    CComBSTR ctrl_id;
    HRESULT hr = rbn_ctrl->get_Id(&ctrl_id);
    UINT imageId = 0;
    if (ctrl_id == _T("LoginButton"))
    {
        if (CSumsAddin::GetAddin()->GetDoc()->GetLoginResult())
            imageId = IDB_PNG_LOGOUT;
        else
            imageId = IDB_PNG_LOGIN;
    }
    else if (ctrl_id == _T("ConfigButton"))
    {
        imageId = IDB_PNG_CONFIG;
    }
    else if (ctrl_id == _T("ManSendButton"))
    {
        imageId = IDB_PNG_MANSEND;
    }
    else
    {
        return nullptr;
    }

    HBITMAP hBitmap = LoadImageFromResource(imageId);
    PICTDESC pictdesc = { 0 };
    pictdesc.cbSizeofstruct = sizeof(PICTDESC);
    pictdesc.picType = PICTYPE_BITMAP;
    pictdesc.bmp.hbitmap = hBitmap;
    pictdesc.bmp.hpal = NULL;
    IPictureDisp* pReturn = nullptr;
    ::OleCreatePictureIndirect(&pictdesc, IID_IPictureDisp, TRUE, (void**)&pReturn);
    return pReturn;
}

STDMETHODIMP_(LONG) CUiMgr::RbnGetSize(IDispatch* pDispCtrl)
{
    return 0;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetDesc(IDispatch* pDispCtrl)
{
    CComQIPtr<IRibbonControl> rbn_ctrl = pDispCtrl;
    ATLASSERT(rbn_ctrl);

    CComBSTR ctrl_id;
    HRESULT hr = rbn_ctrl->get_Id(&ctrl_id);
    if (ctrl_id == _T("LoginButton"))
    {
        CComBSTR desc(L"login button description");
        return desc.Detach();
    }
    else if (ctrl_id == _T("ConfigButton"))
    {
        CComBSTR desc(L"config button description");
        return desc.Detach();
    }
    else if (ctrl_id == _T("ManSendButton"))
    {
        CComBSTR desc(L"manual send button description");
        return desc.Detach();
    }
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetLabel(IDispatch* pDispCtrl)
{
    CComQIPtr<IRibbonControl> rbn_ctrl = pDispCtrl;
    ATLASSERT(rbn_ctrl);

    CComBSTR ctrl_id;
    HRESULT hr = rbn_ctrl->get_Id(&ctrl_id);
    if (ctrl_id == _T("LoginButton"))
    {
        if (CSumsAddin::GetAddin()->GetDoc()->GetLoginResult())
        {
            CString str;
            str.LoadString(IDS_ALREADY_LOGIN);
            CComBSTR label(str);
            return label.Detach();
        }
        else
        {
            CString str;
            str.LoadString(IDS_PLEASE_LOGIN);
            CComBSTR label(str);
            return label.Detach();
        }
    }
    else if (ctrl_id == _T("ConfigButton"))
    {
        CString str;
        str.LoadString(IDS_GLOBAL_CONFIG);
        CComBSTR label(str);
        return label.Detach();
    }
    else if (ctrl_id == _T("ManSendButton"))
    {
        CString str;
        str.LoadString(IDS_MAN_SEND);
        CComBSTR label(str);
        return label.Detach();
    }
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetKeyTip(IDispatch* pDispCtrl)
{
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetScreenTip(IDispatch* pDispCtrl)
{
    CComQIPtr<IRibbonControl> rbn_ctrl = pDispCtrl;
    ATLASSERT(rbn_ctrl);

    CComBSTR ctrl_id;
    HRESULT hr = rbn_ctrl->get_Id(&ctrl_id);
    if (ctrl_id == _T("LoginButton"))
    {
        if (CSumsAddin::GetAddin()->GetDoc()->GetLoginResult())
        {
            CString str;
            str.LoadString(IDS_ALREADY_LOGIN);
            CComBSTR screen_tip(str);
            return screen_tip.Detach();
        }
        else
        {
            CString str;
            str.LoadString(IDS_PLEASE_LOGIN);
            CComBSTR screen_tip(str);
            return screen_tip.Detach();
        }
    }
    else if (ctrl_id == _T("ConfigButton"))
    {
        CString str;
        str.LoadString(IDS_GLOBAL_CONFIG);
        CComBSTR screen_tip(str);
        return screen_tip.Detach();
    }
    else if (ctrl_id == _T("ManSendButton"))
    {
        CString str;
        str.LoadString(IDS_MAN_SEND);
        CComBSTR screen_tip(str);
        return screen_tip.Detach();
    }
    return nullptr;
}

STDMETHODIMP_(BSTR) CUiMgr::RbnGetSuperTip(IDispatch* pDispCtrl)
{
    CComQIPtr<IRibbonControl> rbn_ctrl = pDispCtrl;
    ATLASSERT(rbn_ctrl);

    CComBSTR ctrl_id;
    HRESULT hr = rbn_ctrl->get_Id(&ctrl_id);
    if (ctrl_id == _T("LoginButton"))
    {
        if (CSumsAddin::GetAddin()->GetDoc()->GetLoginResult())
        {
            CString str;
            str.LoadString(IDS_ALREADY_LOGIN);
            CComBSTR super_tip(str);
            return super_tip.Detach();
        }
        else
        {
            CString str;
            str.LoadString(IDS_PLEASE_LOGIN);
            CComBSTR super_tip(str);
            return super_tip.Detach();
        }
    }
    else if (ctrl_id == _T("ConfigButton"))
    {
        CString str;
        str.LoadString(IDS_GLOBAL_CONFIG_TIP);
        CComBSTR super_tip(str);
        return super_tip.Detach();
    }
    else if (ctrl_id == _T("ManSendButton"))
    {
        CString str;
        str.LoadString(IDS_MAN_SEND_TIP);
        CComBSTR super_tip(str);
        return super_tip.Detach();
    }
    return nullptr;
}

STDMETHODIMP_(VARIANT_BOOL) CUiMgr::RbnGetEnabled(IDispatch* pDispCtrl)
{
    CComQIPtr<IRibbonControl> rbn_ctrl = pDispCtrl;
    ATLASSERT(rbn_ctrl);

    CComBSTR ctrl_id;
    HRESULT hr = rbn_ctrl->get_Id(&ctrl_id);
    if (ctrl_id == _T("LoginButton"))
    {

    }
    else if (ctrl_id == _T(""))
    {

    }
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

BSTR CUiMgr::GetXmlResource(int nId)
{
    LPVOID pResourceData = nullptr; DWORD dwSizeInBytes = 0;
    HRESULT hr = GetResource(nId, TEXT("XML"), &pResourceData, &dwSizeInBytes);
    if ( FAILED(hr) )
        return nullptr;

    // Assumes that the data is not stored in Unicode.
    CComBSTR cbstr(dwSizeInBytes, reinterpret_cast<LPCSTR>(pResourceData));
    return cbstr.Detach();
}

HBITMAP CUiMgr::LoadImageFromResource(UINT nResourceId, LPCTSTR pszResType/* = _T("PNG") */)
{
    HBITMAP hBitmap = NULL;
    HRSRC hRsrc = ::FindResource(_AtlBaseModule.GetModuleInstance(), MAKEINTRESOURCE(nResourceId), pszResType);
    if ( hRsrc == NULL )
        return hBitmap;

    // load resource into memory
    DWORD len = SizeofResource(_AtlBaseModule.GetModuleInstance(), hRsrc);
    BYTE* lpRsrc = (BYTE*)LoadResource(_AtlBaseModule.GetModuleInstance(), hRsrc);
    if (lpRsrc == NULL)
        return hBitmap;

    // allocate global memory on which to create stream
    HGLOBAL hGlbMem = GlobalAlloc(GMEM_FIXED, len);
    BYTE* pByte = (BYTE*)GlobalLock(hGlbMem);
    memcpy(pByte, lpRsrc, len);
    IStream* pStm;
    CreateStreamOnHGlobal(hGlbMem, FALSE, &pStm);

    // load from stream
    CImage image;
    image.Load(pStm);
    hBitmap = image.Detach();

    // release tmp buffer
    GlobalUnlock(hGlbMem);
    FreeResource(lpRsrc);
    if (pStm)
        pStm->Release();

    return hBitmap;
}

void CUiMgr::CaptureExcelMainHwnd()
{
    //::EnumThreadWindows(::GetCurrentThreadId(), EnumThreadHwnd, (LPARAM)&m_MainWnd);
}

//BOOL __stdcall CUiMgr::EnumThreadHwnd(HWND hwnd, LPARAM lpParam)
//{
//    char class_name[256] = {0};
//    ::GetClassNameA(hwnd, class_name, 256);
//    if (0 == _stricmp(class_name, "XLMAIN"))
//    {
//        *(HWND*)lpParam = hwnd;
//        return FALSE; // stop enum
//    }
//    return TRUE; // continue enum
//}