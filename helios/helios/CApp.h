//
//  CApp.h
//
//  Created by Colin on 2020-03-02.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#pragma once
#include "lava_base.h"
#include "ConnHelper.h"

using namespace ATL;

#define AppEventSinkID	7
typedef IDispEventSimpleImpl< AppEventSinkID, class CApp, &__uuidof(AppEvents) >	AppEventSinker;

struct CallerInfo
{
	CString			BookName;
	CString			SheetName;
	long			Col;
	long			Row;
	CComPtr<Range>	Caller;
};

class CApp : public AppEventSinker,
			 public CConnHelper<CApp>
{
public:
	CApp(LPDISPATCH Application);
	~CApp();

	inline CComPtr<_Application> get_Application() { return m_spApp; }
	CComPtr<IDispatch> get_Caller();
	BOOL get_CallerInfo(CallerInfo* info);
	void CalculateFull();
	void RefreshSingle(Range* lpDisp);

	//
	// Events Sinker: AppEventSinkID
	//
	BEGIN_SINK_MAP(CApp)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x616, OnSheetSelectionChange, &SheetSelectionChangeInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x617, OnSheetBeforeDoubleClick, &SheetBeforeDoubleClickInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x618, OnSheetBeforeRightClick, &SheetBeforeRightClickInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x619, OnSheetActivate, &SheetActivateInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x61a, OnSheetDeactivate, &SheetDeactivateInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x61b, OnSheetCalculate, &SheetCalculateInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x61c, OnSheetChange, &SheetChangeInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0x61d, OnNewWorkbook, &NewWorkbookInfo)
		SINK_ENTRY_INFO(AppEventSinkID, __uuidof(AppEvents), 0xa34, OnAfterCalculate, &AfterCalculateInfo)
	END_SINK_MAP()

	//
	// AppEvents
	//
	void __stdcall OnSheetSelectionChange(__lv_in IDispatch* Sh, __lv_in Range* Target);
	void __stdcall OnSheetBeforeDoubleClick(__lv_in IDispatch* Sh, __lv_in Range* Target, __lv_inout VARIANT_BOOL* Cancel);
	void __stdcall OnSheetBeforeRightClick(__lv_in IDispatch* Sh, __lv_in Range* Target, __lv_inout VARIANT_BOOL* Cancel);
	void __stdcall OnSheetActivate(__lv_in IDispatch* Sh);
	void __stdcall OnSheetDeactivate(__lv_in IDispatch* Sh);
	void __stdcall OnSheetCalculate(__lv_in IDispatch* Sh);
	void __stdcall OnSheetChange(__lv_in IDispatch* Sh, __lv_in Range* Target);
	void __stdcall OnNewWorkbook(__lv_in _Workbook* Wb);
	void __stdcall OnAfterCalculate();

private:
	static _ATL_FUNC_INFO SheetSelectionChangeInfo;
	static _ATL_FUNC_INFO SheetBeforeDoubleClickInfo;
	static _ATL_FUNC_INFO SheetBeforeRightClickInfo;
	static _ATL_FUNC_INFO SheetActivateInfo;
	static _ATL_FUNC_INFO SheetDeactivateInfo;
	static _ATL_FUNC_INFO SheetCalculateInfo;
	static _ATL_FUNC_INFO SheetChangeInfo;	
	static _ATL_FUNC_INFO NewWorkbookInfo;
	static _ATL_FUNC_INFO AfterCalculateInfo;

	CComQIPtr< _Application >	m_spApp;
};