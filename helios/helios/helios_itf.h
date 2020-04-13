#pragma once
#include "windows.h"
#include "lava_base.h"

//
// 0002441b-0000-0000-c000-000000000046  RefreshEvents
//		QueryTable;
//			[ default ] dispinterface _QueryTable
//			[ default, source ] dispinterface RefreshEvents
//
// 00024413-0000-0000-C000-000000000046  AppEvents
//		Application;
//			[ default ] interface _Application
//			[ default, source ] dispinterface AppEvents
//
// 00024412-0000-0000-c000-000000000046  WorkbookEvents
//		Workbook;
//			[ default ] interface _Workbook
//			[ default, source ] dispinterface WorkbookEvents
//
// 00024411-0000-0000-c000-000000000046  DocEvents
//		Worksheet;
//			[ default ] interface _Worksheet
//			[ default, source ] dispinterface DocEvents
//
// 00024410-0000-0000-c000-000000000046  OLEObjectEvents
//		OLEObject;
//			[ default ] dispinterface _OLEObject
//			[ default, source ] dispinterface OLEObjectEvents
//
// 0002440f-0000-0000-c000-000000000046  ChartEvents
//		Chart;
//			[ default ] interface _Chart
//			[ default, source ] dispinterface ChartEvents
//

//
// Excel Error Code Table:
//		0x80020010			DISP_E_BADCALLEE		
//		0x800A03EC			NAME_NOT_FOUND
//		0x800AC472			VBA_E_IGNORE
//

#define WM_DATA_ARRIVED     WM_USER + 0x100
#define WM_LOGIN			WM_USER + 0x101
#define WM_LOGOUT           WM_USER + 0x102

struct DECLSPEC_UUID("DF1E18FC-45FD-4859-BB19-DC740D198FA6") IUiMgr : public IUnknown
{
	virtual void __stdcall Initialize() = 0;
	virtual void __stdcall InvalidateControl(__lv_in BSTR ctrlId) = 0;
	virtual void __stdcall InvalidateControl(__lv_in UINT ctrlId) = 0;
	virtual void __stdcall PostCustomMessage(__lv_in UINT Msg, __lv_in WPARAM wParam, __lv_in LPARAM lParam) = 0;
};
