#pragma once
#include "windows.h"

struct DECLSPEC_UUID("DF1E18FC-45FD-4859-BB19-DC740D198FA6") IUiMgr : public IUnknown
{
	virtual void __stdcall CaptureExcelMainHwnd() = 0;
};
