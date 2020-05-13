#include "stdafx.h"
#include <iomanip>
#include <regex>
#include "CDoc.h"
#include "lava_crt.h"
#include "ConfigDlg.h"

#define HELIOS_MAX_FREQUENCY_TIMESPAN           86399   // 24hours

LRESULT CConfigDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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

    m_editExChangedFt.SubclassWindow((CEdit)GetDlgItem(IDC_FT_PRECISION));
    m_editFrqTimeSpan = (CEdit)GetDlgItem(IDC_FREQUENCY_SPAN);

    // get global config info
    int ret = CSumsAddin::GetAddin()->GetDoc()->GetGlobalConfigInfo(m_global_info);
    if (0 == ret)
    {
        // Set changed format precision
        m_editExChangedFt.SetWindowTextW(CA2T(m_global_info.m_changedSendInfo.m_formatPrecision.c_str()));

        // Set timeSpan and the frequent flag
        CString timeSpan;
        timeSpan.Format(_T("%d"), m_global_info.m_FreqSendInfo.m_defaultSendTimeSpan);
        m_editFrqTimeSpan.SetWindowTextW(timeSpan);
        ((CButton)GetDlgItem(IDC_CHK_FRQ_SEND_NOW)).SetCheck(m_global_info.m_FreqSendInfo.m_sendDataRightNow ? BST_CHECKED : BST_UNCHECKED);
    }

    return TRUE;
}

LRESULT CConfigDlg::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CPaintDC dc(m_hWnd);
    RECT rect;
    GetClientRect(&rect);
    HDC hDC = dc.m_hDC;
    Gdiplus::Graphics gph(hDC);
    gph.DrawImage(m_pImage, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
    return TRUE;
}

LRESULT CConfigDlg::OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    HDC hDC = (HDC)wParam;
    SetBkColor(hDC, RGB(27, 59, 44));
    SetTextColor(hDC, RGB(255, 255, 255));
    return (LRESULT)GetStockObject(NULL_BRUSH);
}

LRESULT CConfigDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // get global config info
    CString csFtPrecision;
    std::string sFtPrecision;
    GetDlgItemText(IDC_FT_PRECISION, csFtPrecision);
    sFtPrecision = CT2A(csFtPrecision.GetString());
    UINT sendTimeSpan = GetDlgItemInt(IDC_FREQUENCY_SPAN);
    bool sendNowFlg = ((CButton)GetDlgItem(IDC_CHK_FRQ_SEND_NOW)).GetCheck() == BST_CHECKED;

    CString boxString;
    CString boxCaption;

    // check format precision
    std::regex rull("^[1-9]\\d*\\.[1-9]\\d*|0\\.[1-9]\\d*$");
    std::smatch matResult;
    if (!std::regex_match(sFtPrecision, matResult, rull))
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_CONFIG_PRECISION);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_CONFIG);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }

    // check the value of time span
    if ((0 == sendTimeSpan) || (sendTimeSpan > HELIOS_MAX_FREQUENCY_TIMESPAN))
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_CONFIG_FREQTIMESPAN);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_CONFIG);
        CString errorString;
        errorString.Format(boxString, HELIOS_MAX_FREQUENCY_TIMESPAN);
        ::MessageBox(this->m_hWnd, errorString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }

    // check changed
    if ((m_global_info.m_changedSendInfo.m_formatPrecision == sFtPrecision)
        && (m_global_info.m_FreqSendInfo.m_defaultSendTimeSpan == sendTimeSpan)
        && (m_global_info.m_FreqSendInfo.m_sendDataRightNow == sendNowFlg))
    {
        boxString.LoadString(IDS_MSGBOX_INFO_CONFIG_NOCHANGE);
        boxCaption.LoadString(IDS_MSGBOX_INFO_CONFIG);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_OK);
        CloseDialog(wID);
        return 0;
    }

    m_global_info.m_changedSendInfo.m_formatPrecision = sFtPrecision;
    m_global_info.m_FreqSendInfo.m_defaultSendTimeSpan = static_cast<int>(sendTimeSpan);
    m_global_info.m_FreqSendInfo.m_sendDataRightNow = sendNowFlg;
    int ret = CSumsAddin::GetAddin()->GetDoc()->WriteGlobalConfigInfo(m_global_info);
    if (0 != ret)
    {
        boxString.LoadString(IDS_MSGBOX_ERROR_CONFIG_SET_FIALED);
        boxCaption.LoadString(IDS_MSGBOX_ERROR_CONFIG);
        ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_ICONEXCLAMATION);
        return 0;
    }

    boxString.LoadString(IDS_MSGBOX_INFO_CONFIG_SET_SUCCESS);
    boxCaption.LoadString(IDS_MSGBOX_INFO_CONFIG);
    ::MessageBox(this->m_hWnd, boxString, boxCaption, MB_OK);
    CloseDialog(wID);
    return 0;
}

LRESULT CConfigDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CloseDialog(wID);
    return 0;
}

LRESULT CConfigDlg::OnSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    if (IDC_FREQUENCY_SPAN == wID)
    {
        m_editFrqTimeSpan.SetFocus();
        m_editFrqTimeSpan.SetSel(0, -1);
    }

    return TRUE;
}

void CConfigDlg::CloseDialog(int nVal)
{
    Gdiplus::GdiplusShutdown(m_GdiToken);
    EndDialog(nVal);
}
