#include "stdafx.h"
#include <iomanip>
#include "CDoc.h"
#include "lava_crt.h"
#include "LoginDlg.h"

#define HELIOS_FAKE_USER_PASSWORD              _T("********")

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
    int ret = CSumsAddin::GetAddin()->GetDoc()->GetUserAccount(m_ac);
    if (0 == ret)
    {
        m_editUser.SetWindowTextW(m_ac.m_user);
        m_editPwd.SetWindowTextW(HELIOS_FAKE_USER_PASSWORD);
        ((CButton)GetDlgItem(IDC_CHECK_PWD_STORE)).SetCheck(TRUE);
    }
    else if (-3 == ret)
    {
        m_editUser.SetWindowTextW(m_ac.m_user);
        m_editPwd.SetWindowTextW(_T(""));
        ((CButton)GetDlgItem(IDC_CHECK_PWD_STORE)).SetCheck(FALSE);
    }
    else
    {
        m_editUser.SetWindowTextW(_T(""));
        m_editPwd.SetWindowTextW(_T(""));
        ((CButton)GetDlgItem(IDC_CHECK_PWD_STORE)).SetCheck(FALSE);
    }

    ((CButton)GetDlgItem(IDC_CHECK_AUTO_LOGIN)).SetCheck(m_ac.m_autologin);

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
    int autologin = ((CButton)GetDlgItem(IDC_CHECK_AUTO_LOGIN)).GetCheck();
    int storePwd = ((CButton)GetDlgItem(IDC_CHECK_PWD_STORE)).GetCheck();

    CString boxString;
    CString boxCaption;

    if ((m_ac.m_user == user) && (HELIOS_FAKE_USER_PASSWORD == pwd))
    {
        // do nothing
        if (CSumsAddin::GetAddin()->GetDoc()->GetLoginResult())
        {
            if ((m_ac.m_autologin != autologin) || (m_ac.m_storePwd != storePwd))
            {
                m_ac.m_autologin = autologin;
                m_ac.m_storePwd = storePwd;
                int ret = CSumsAddin::GetAddin()->GetDoc()->WriteUserAccount(m_ac);
                if (0 != ret)
                {
                    boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_STORE_FLAG);
                    boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
                    ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
                    return 0;
                }
                boxString.LoadString(IDS_MSGBOX_INFO_LOGIN_SET_SUCCESS);
                boxCaption.LoadString(IDS_MSGBOX_INFO_LOGIN);
                ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_OK);
                CloseDialog(wID);
                return 0;
            }
            boxString.LoadString(IDS_MSGBOX_INFO_LOGIN_ALREADY_LOGIN);
            boxCaption.LoadString(IDS_MSGBOX_INFO_LOGIN);
            ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_OK);
            CloseDialog(wID);
            return 0;
        }
    }

    // check input
    if (user.IsEmpty())
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_USER_EMPTY);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }
    if (pwd.IsEmpty())
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_PSW_EMPTY);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }
    if (user.GetLength() > HELIOS_MAX_UNAME_LEN)
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_USER_TOOLONG);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }
    if (pwd.GetLength() >= HELIOS_MAX_PASSWORD_LEN)
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_PSW_TOOLONG);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }

    // get user account info
    m_ac.m_user = user;
    if (HELIOS_FAKE_USER_PASSWORD != pwd)
    {
        // password convert to md5
        std::string pwdMd5 = Conver2Md5(pwd);
        if (pwdMd5.empty())
        {
            boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_MD5_ENCODE_FAILED);
            boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
            ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
            return 0;
        }
        m_ac.m_password = CA2T(pwdMd5.c_str());
    }

    // get flags of check box
    m_ac.m_autologin = ((CButton)GetDlgItem(IDC_CHECK_AUTO_LOGIN)).GetCheck();
    m_ac.m_storePwd = ((CButton)GetDlgItem(IDC_CHECK_PWD_STORE)).GetCheck();

    // store account info
    int ret = CSumsAddin::GetAddin()->GetDoc()->WriteUserAccount(m_ac);
    if (0 != ret)
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_STORE_ACCOUNT);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }

    // login to CDH
    bool res = CSumsAddin::GetAddin()->GetDoc()->LoginCDH(m_ac);
    if (res)
    {
        CSumsAddin::GetAddin()->GetDoc()->SetLoginResult(true);
        boxString.LoadString(IDS_MSGBOX_INFO_LOGIN_SUCCESS);
        boxCaption.LoadString(IDS_MSGBOX_INFO_LOGIN);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_OK);
    }
    else
    {
        CSumsAddin::GetAddin()->GetDoc()->SetLoginResult(false);
        boxString.LoadString(IDS_MSGBOX_ERROR_LOGIN_FAILED);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_LOGIN);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
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
