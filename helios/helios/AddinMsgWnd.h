#pragma once
#include "atlwin.h"

typedef ATL::CWinTraits<WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, 0>	MsgWinTraits;

class AddinMsgWnd : public ATL::CWindowImpl<AddinMsgWnd, ATL::CWindow, MsgWinTraits>
{
public:
    AddinMsgWnd() {};
    ~AddinMsgWnd() { DestroyWindow(); }

    void Initialize(HWND parent);

    BEGIN_MSG_MAP(CLoginDlg)
        MESSAGE_HANDLER(WM_DATA_ARRIVED, OnDataArrived)
        MESSAGE_HANDLER(WM_LOGIN, OnLogIn)
        MESSAGE_HANDLER(WM_LOGOUT, OnLogOut)
    END_MSG_MAP()

    LRESULT OnDataArrived(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnLogIn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
    LRESULT OnLogOut(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/);
};

