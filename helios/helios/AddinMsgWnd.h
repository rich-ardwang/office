#pragma once
#include "atlwin.h"

typedef ATL::CWinTraits<WS_POPUP | WS_BORDER | WS_CAPTION, 0>	MsgWinTraits;

class AddinMsgWnd : public ATL::CWindowImpl<AddinMsgWnd, ATL::CWindow, MsgWinTraits>
{
public:
    AddinMsgWnd() {};
    ~AddinMsgWnd() { DestroyWindow(); }

    void Initialize(HWND parent);

    BEGIN_MSG_MAP(CLoginDlg)
        MESSAGE_HANDLER(WM_LOGIN, OnLogIn)
        MESSAGE_HANDLER(WM_LOGOUT, OnLogOut)
        MESSAGE_HANDLER(WM_DATA_ARRIVED, OnDataArrived)
        MESSAGE_HANDLER(WM_REFRESH, OnRefresh)
    END_MSG_MAP()

    LRESULT OnLogIn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnLogOut(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnDataArrived(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnRefresh(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
};

