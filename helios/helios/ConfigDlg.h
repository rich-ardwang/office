// ConfigDlg.h : interface of the CConfigDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <GdiPlus.h>
#include <atlimage.h>
#include "SumsAddin.h"
#include "CEditEx.h"

#pragma comment(lib, "GdiPlus.lib")

class CConfigDlg : public ATL::CDialogImpl<CConfigDlg>
{
public:
    CConfigDlg() : m_pImage(nullptr) {}
    ~CConfigDlg()
    {
        if (m_pImage)
        {
            delete m_pImage;
            m_pImage = nullptr;
        }
    }

    enum { IDD = IDD_DLG_CONFIG };

    BEGIN_MSG_MAP(CConfigDlg)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_CTLCOLORSTATIC, OnCtlColorStatic)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
        COMMAND_CODE_HANDLER(EN_SETFOCUS, OnSetFocus)
    END_MSG_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnCtlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnSetFocus(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    void CloseDialog(int nVal);

private:
    Gdiplus::Image*     m_pImage;
    ULONG_PTR           m_GdiToken;
    std::wstring        m_ModulePath;
    GlobalInfo          m_global_info;
    CEditEx             m_editExChangedFt;
    CEdit               m_editFrqTimeSpan;
};
