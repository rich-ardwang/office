#include "stdafx.h"
#include "SumsAddin.h"
#include "CUiMgr.h"
#include "AddinMsgWnd.h"

void AddinMsgWnd::Initialize(HWND parent)
{
	Create(parent);
}

LRESULT AddinMsgWnd::OnDataArrived(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CSumsAddin::GetAddin()->GetDoc()->OnDataArrived();
	return 0;
}

LRESULT AddinMsgWnd::OnLogIn(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CSumsAddin::GetAddin()->GetUiMgr()->InvalidateControl(_T("LoginButton"));
	return 0;
}

LRESULT AddinMsgWnd::OnLogOut(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	CSumsAddin::GetAddin()->GetUiMgr()->InvalidateControl(_T("LoginButton"));
	return 0;
}
