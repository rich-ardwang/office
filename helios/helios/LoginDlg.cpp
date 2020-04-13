#include "stdafx.h"
#include <iomanip>
#include "CDoc.h"
#include "lava_crt.h"
#include "LoginDlg.h"

#define FAKE_USER_PASSWORD              _T("********")

LRESULT CLoginDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    // center the dialog on the screen
    CenterWindow();

    // set icons
    HICON hIcon = AtlLoadIconImage(IDI_ICON_MAIN, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
    SetIcon(hIcon, TRUE);
    HICON hIconSmall = AtlLoadIconImage(IDI_ICON_MAIN, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
    SetIcon(hIconSmall, FALSE);

    // start GDI plus
    Gdiplus::GdiplusStartupInput gdiInput = { 0 };
    GdiplusStartup(&m_GdiToken, &gdiInput, NULL);

    m_ModulePath = CSumsAddin::GetAddin()->GetModulePath();
    std::wstring imagePath = m_ModulePath + L"sumscope.jpg";
    m_pImage = new Gdiplus::Image(imagePath.c_str());

    m_editUser = (CEdit)GetDlgItem(IDC_EDIT_USER);
    m_editPwd = (CEdit)GetDlgItem(IDC_EDIT_PSWD);
    m_editPwd.SetPasswordChar(_T('*'));

    // get user account info
    bool ret = CSumsAddin::GetAddin()->GetDoc()->GetUserAccount(m_ac);
    if (ret)
    {
        m_editUser.SetWindowTextW(m_ac.m_user);
        m_editPwd.SetWindowTextW(FAKE_USER_PASSWORD);
        ((CButton)GetDlgItem(IDC_CHECK_ACCOUNT_STORE)).SetCheck(TRUE);
    }
    else
        ((CButton)GetDlgItem(IDC_CHECK_ACCOUNT_STORE)).SetCheck(FALSE);

    return TRUE;
}

LRESULT CLoginDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CPaintDC dc(m_hWnd);
    RECT rect;
    GetClientRect(&rect);
    HDC hDC = dc.m_hDC;
    Gdiplus::Graphics gph(hDC);
    gph.DrawImage(m_pImage, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
    return TRUE;
}

LRESULT CLoginDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    HDC hDC = (HDC)wParam;
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, RGB(255, 255, 255));
    return (LRESULT)GetStockObject(NULL_BRUSH);
}

LRESULT CLoginDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // get the account info
    CString user = _T("");
    CString pwd = _T("");
    m_editUser.GetWindowTextW(user);
    m_editPwd.GetWindowTextW(pwd);

    if ((m_ac.m_user == user) && (FAKE_USER_PASSWORD == pwd))
    {
        // do nothing
        if (CSumsAddin::GetAddin()->GetDoc()->GetLoginResult())
        {
            ::MessageBox(this->m_hWnd, L"Already login.", L"MessageBox", MB_OK);
            CloseDialog(wID);
            return 0;
        }
    }

    // check input
    if (user.IsEmpty())
    {
        ::MessageBox(this->m_hWnd, L"User name is empty!", L"Input Error", MB_ICONEXCLAMATION);
        return 0;
    }
    if (pwd.IsEmpty())
    {
        ::MessageBox(this->m_hWnd, L"Password is empty!", L"Input Error", MB_ICONEXCLAMATION);
        return 0;
    }
    if (user.GetLength() > HELIOS_MAX_UNAME_LEN)
    {
        ::MessageBox(this->m_hWnd, L"User name too long!", L"Input Error", MB_ICONEXCLAMATION);
        return 0;
    }
    if (pwd.GetLength() >= HELIOS_MAX_PASSWORD_LEN)
    {
        ::MessageBox(this->m_hWnd, L"Password too long!", L"Input Error", MB_ICONEXCLAMATION);
        return 0;
    }

    // get user account info
    m_ac.m_user = user;
    if (FAKE_USER_PASSWORD != pwd)
    {
        // password convert to md5
        std::string pwdMd5 = Conver2Md5(pwd);
        if (pwdMd5.empty())
        {
            ::MessageBox(this->m_hWnd, L"md5 encode for password failed!", L"Input Error", MB_ICONEXCLAMATION);
            return 0;
        }
        m_ac.m_password = CA2T(pwdMd5.c_str());
    }

    // check the flag and store account info
    int chkState = ((CButton)GetDlgItem(IDC_CHECK_ACCOUNT_STORE)).GetCheck();
    if (TRUE == chkState)
    {
        bool ret = CSumsAddin::GetAddin()->GetDoc()->WriteUserAccount(m_ac);
        if (!ret)
        {
            ::MessageBox(this->m_hWnd, L"Store account failed!", L"Login Error", MB_ICONEXCLAMATION);
            return 0;
        }
    }
    else
    {
        bool ret = CSumsAddin::GetAddin()->GetDoc()->ClearUserAccount();
        if (!ret)
        {
            ::MessageBox(this->m_hWnd, L"Clear account failed!", L"Login Error", MB_ICONEXCLAMATION);
            return 0;
        }
    }

    // login to CDH
    bool ret = CSumsAddin::GetAddin()->GetDoc()->LoginCDH(m_ac);
    if (ret)
    {
        CSumsAddin::GetAddin()->GetDoc()->SetLoginResult(true);
        ::MessageBox(this->m_hWnd, L"Login success.", L"MessageBox", MB_OK);
    }
    else
    {
        CSumsAddin::GetAddin()->GetDoc()->SetLoginResult(false);
        ::MessageBox(this->m_hWnd, L"Login failed!", L"Login Error", MB_ICONEXCLAMATION);
        return 0;
    }

    CloseDialog(wID);
    return 0;
}

LRESULT CLoginDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CloseDialog(wID);
    return 0;
}

LRESULT CLoginDlg::OnSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (IDC_EDIT_USER == wID)
    {
        m_editUser.SetFocus();
        m_editUser.SetSel(0, -1);
    }
    else if (IDC_EDIT_PSWD == wID)
    {
        m_editPwd.SetFocus();
        m_editPwd.SetSel(0, -1);
    }

    return TRUE;
}

void CLoginDlg::CloseDialog(int nVal)
{
    Gdiplus::GdiplusShutdown(m_GdiToken);
    EndDialog(nVal);
}

std::string CLoginDlg::Conver2Md5(const CString &in)
{
    std::string stdIn = CT2A(in.GetString());
    std::string trimIn = lava::utils::trim(stdIn);
    char md5Value[33];
    uint32_t len = 33;
    int ret = md5_encode(trimIn.c_str(), trimIn.length(), md5Value, len);
    if (0 == ret)
    {
        std::string retValue(md5Value);
        return retValue;
    }
    else
        return "";
}
