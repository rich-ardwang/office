// CEditEx.h : advanced edit control
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

class CEditEx : public ATL::CWindowImpl<CEditEx>
{
public:
    CEditEx() {}
    virtual ~CEditEx() {}

    DECLARE_WND_SUPERCLASS(_T("EDIT_EX"), _T("EDIT"))

    BEGIN_MSG_MAP(CEditEx)
        MESSAGE_HANDLER(WM_CHAR, OnChar)
    END_MSG_MAP()

public:
    LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};
