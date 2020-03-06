// LoginDlg.h : interface of the CLoginDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include <GdiPlus.h>
#include <atlimage.h>

#pragma comment(lib, "GdiPlus.lib")
Gdiplus::GdiplusStartupInput g_GdiInput;
ULONG g_GdiToken = 0;

class CLoginDlg : public ATL::CDialogImpl<CLoginDlg>
{
public:
    CLoginDlg(HWND pWnd = nullptr) : m_pMainWnd(pWnd),
                                     m_pImage(nullptr) {}
    ~CLoginDlg()
    {
        if (m_pImage)
        {
            delete m_pImage;
            m_pImage = nullptr;
        }
    }

    enum { IDD = IDD_DLG_LOGIN };

    BEGIN_MSG_MAP(CLoginDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        // center the dialog on the screen
        CenterWindow();

        // set icons
        HICON hIcon = AtlLoadIconImage(IDI_ICON_MAIN, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
        SetIcon(hIcon, TRUE);
        HICON hIconSmall = AtlLoadIconImage(IDI_ICON_MAIN, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
        SetIcon(hIconSmall, FALSE);

        // start GDI plus
        GdiplusStartup(&g_GdiToken, &g_GdiInput, NULL);

        // load image
        TCHAR szPath[MAX_PATH];
        HMODULE hModule = _AtlBaseModule.GetModuleInstance();
        if (!hModule)
        {
            //printf("GetModuleInstance failed (%d)\n", GetLastError());
            return FALSE;
        }
        if (!GetModuleFileName(hModule, szPath, MAX_PATH))
        {
            //printf("GetModuleFileName failed (%d)\n", GetLastError());
            return FALSE;
        }
        PathRemoveFileSpec(szPath);
        CString imagePath(szPath);
        imagePath += _T("\\sumscope.jpg");
        m_pImage = new Gdiplus::Image(imagePath);

        // set check box
        ((CButton)GetDlgItem(IDC_CHECK_ACCOUNT_STORE)).SetCheck(TRUE);

        return TRUE;
    }

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);
        RECT rect;
        GetClientRect(&rect);
        HDC hDC = dc.m_hDC;
        Gdiplus::Graphics gph(hDC);
        gph.DrawImage(m_pImage, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
        return TRUE;
    }

    LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        HDC hDC = (HDC)wParam;
        SetBkMode(hDC, TRANSPARENT);
        SetTextColor(hDC, RGB(255, 255, 255));
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }

    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        // get the account info
        CString user = _T("");
        CString pwd = _T("");
        GetDlgItemText(IDC_EDIT_USER, user);
        GetDlgItemText(IDC_EDIT_PSWD, pwd);

        // check input
        if (user.IsEmpty())
        {
            ::MessageBox(m_pMainWnd, L"User name is empty!", L"Input Error", MB_ICONEXCLAMATION);
            return 0;
        }
        if (pwd.IsEmpty())
        {
            ::MessageBox(m_pMainWnd, L"Password is empty!", L"Input Error", MB_ICONEXCLAMATION);
            return 0;
        }
        if (user.GetLength() > 32)
        {
            ::MessageBox(m_pMainWnd, L"User name too long!", L"Input Error", MB_ICONEXCLAMATION);
            return 0;
        }
        if (pwd.GetLength() >= 32)
        {
            ::MessageBox(m_pMainWnd, L"Password too long!", L"Input Error", MB_ICONEXCLAMATION);
            return 0;
        }

        // password convert to md5
        CString pwdMd5 = _T("");

        // get check box
        int chkState = ((CButton)GetDlgItem(IDC_CHECK_ACCOUNT_STORE)).GetCheck();
        if (TRUE == chkState)
        {
            // need store the user's account
            // todo: store the user name and the md5 value of the password
        }

        // todo: login to CDH
        if (0)
        {
            ::MessageBox(m_pMainWnd, L"Login success.", L"MessageBox", MB_OK);
            CloseDialog(wID);
        }
        else
        {
            ::MessageBox(m_pMainWnd, L"Login failed!", L"Login Error", MB_ICONEXCLAMATION);
        }

        return 0;
    }

    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        CloseDialog(wID);
        return 0;
    }

    void CloseDialog(int nVal)
    {
        Gdiplus::GdiplusShutdown(g_GdiToken);
        EndDialog(nVal);
    }

private:
    Gdiplus::Image      *m_pImage;
    HWND                m_pMainWnd;
};
