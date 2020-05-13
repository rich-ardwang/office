#include "stdafx.h"
#include "CEditEx.h"

LRESULT CEditEx::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    char nChar = (char)wParam;

    // It can only input num 0-9, or backspace(VK_BACK), or decimal dot(VK_DELETE)
    // WARN: decimal dot is VK_DELETE, not VK_DECIMAL
    if (('0' <= nChar && nChar <= '9')
        || (VK_BACK == nChar)
        || (VK_DELETE == nChar))
    {
        bHandled = FALSE;
    }

    return 0;
}
