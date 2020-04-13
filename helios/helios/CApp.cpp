//
//  CApp.cpp
//
//  Created by Colin on 2020-03-02.
//  Copyright (c) 2020 Sumscope. All rights reserved.
//
#include "stdafx.h"
#include "CApp.h"
#include "SumsAddin.h"
#include "lava_utils_api.h"

_ATL_FUNC_INFO CApp::SheetSelectionChangeInfo = { CC_STDCALL, VT_EMPTY, 2, { VT_BYREF | VT_DISPATCH, VT_BYREF | VT_PTR  } };
_ATL_FUNC_INFO CApp::SheetBeforeDoubleClickInfo = { CC_STDCALL, VT_EMPTY, 3, { VT_BYREF | VT_DISPATCH, VT_BYREF | VT_PTR, VT_BYREF | VT_PTR | VT_BOOL } };
_ATL_FUNC_INFO CApp::SheetBeforeRightClickInfo = { CC_STDCALL, VT_EMPTY, 3, { VT_BYREF | VT_DISPATCH, VT_BYREF | VT_PTR, VT_BYREF | VT_PTR | VT_BOOL } };
_ATL_FUNC_INFO CApp::SheetActivateInfo = { CC_STDCALL, VT_EMPTY, 1, { VT_BYREF | VT_DISPATCH } };
_ATL_FUNC_INFO CApp::SheetDeactivateInfo = { CC_STDCALL, VT_EMPTY, 1, { VT_BYREF | VT_DISPATCH } };
_ATL_FUNC_INFO CApp::SheetCalculateInfo = { CC_STDCALL, VT_EMPTY, 1, { VT_BYREF | VT_DISPATCH } };
_ATL_FUNC_INFO CApp::SheetChangeInfo = { CC_STDCALL, VT_EMPTY, 2, { VT_BYREF | VT_DISPATCH, VT_BYREF | VT_PTR } };
_ATL_FUNC_INFO CApp::NewWorkbookInfo = { CC_STDCALL, VT_EMPTY, 1, { VT_BYREF | VT_PTR } };
_ATL_FUNC_INFO CApp::AfterCalculateInfo = { CC_STDCALL, VT_EMPTY, 0, { VT_EMPTY } };

CApp::CApp(LPDISPATCH Application)
{
	m_spApp = Application;
	if ( m_spApp )
		PlugInSinkerToSvr<AppEventSinker>(m_spApp);
	else
		log_error(helios_module, "Application Dispatch Interface is NULL.");
}

CApp::~CApp()
{
	if ( m_spApp )
	{
		PlugOutSinkerFromSvr<AppEventSinker>(m_spApp);
		m_spApp = nullptr;
	}
}

CComPtr<IDispatch> CApp::get_Caller()
{
	VARIANT val;
	HRESULT hr = m_spApp->get_Caller(CComVariant(1), 0, &val);
	if (SUCCEEDED(hr))
	{
		CComPtr<IDispatch> caller;
		caller.Attach(val.pdispVal);
		return caller;
	}
	return nullptr;
}

BOOL CApp::get_CallerInfo(CallerInfo* info)
{
	ATLASSERT(info);
	CComPtr<IDispatch> spDisp = get_Caller();
	if ( spDisp )
	{
		CComQIPtr<Range> spRange(spDisp);
		_WorksheetPtr sheet = spRange->GetWorksheet();
		_WorkbookPtr book = sheet->Parent;
		info->BookName = (wchar_t*)book->GetName();
		info->SheetName = (wchar_t*)sheet->GetName();
		info->Row = spRange->GetRow();
		info->Col = spRange->GetColumn();
		info->Caller = spRange;
		return TRUE;
	}
	return FALSE;
}

void CApp::CalculateFull()
{
	m_spApp->raw_CalculateFull();
}

void CApp::RefreshSingle(Range* lpDisp)
{
	_variant_t formula = lpDisp->GetFormulaLocal();
	if (formula != _variant_t(_T("")))
	{		
		lpDisp->PutFormulaLocal(_variant_t(_T(" ")));
		lpDisp->PutFormulaLocal(formula);
	}
	else
	{
		formula = lpDisp->GetFormulaArray();
		lpDisp->PutFormulaArray(_variant_t(_T(" ")));
		lpDisp->PutFormulaArray(formula);
	}
}

void __stdcall CApp::OnSheetSelectionChange(__lv_in IDispatch* Sh, __lv_in Range* Target)
{
	log_info(helios_module, "Enter OnSheetSelectionChange");
	log_info(helios_module, "Leave OnSheetSelectionChange");
}

void __stdcall CApp::OnSheetBeforeDoubleClick(__lv_in IDispatch* Sh, __lv_in struct Range* Target, __lv_inout VARIANT_BOOL* Cancel)
{
	log_info(helios_module, "Enter OnSheetBeforeDoubleClick");
	log_info(helios_module, "Leave OnSheetBeforeDoubleClick");
}

void __stdcall CApp::OnSheetBeforeRightClick(__lv_in IDispatch* Sh, __lv_in struct Range* Target, __lv_inout VARIANT_BOOL* Cancel)
{
	log_info(helios_module, "Enter OnSheetBeforeRightClick");
	log_info(helios_module, "Leave OnSheetBeforeRightClick");
}

void __stdcall CApp::OnSheetActivate(__lv_in IDispatch* Sh)
{
	log_info(helios_module, "Enter OnSheetActivate");
	log_info(helios_module, "Leave OnSheetActivate");
}

void __stdcall CApp::OnSheetDeactivate(__lv_in IDispatch* Sh)
{
	log_info(helios_module, "Enter OnSheetDeactivate");
	log_info(helios_module, "Leave OnSheetDeactivate");
}

void __stdcall CApp::OnSheetCalculate(__lv_in IDispatch* Sh)
{
	log_info(helios_module, "Enter OnSheetCalculate");
	CSumsAddin::GetAddin()->GetDoc()->OnCalculate();
	log_info(helios_module, "Leave OnSheetCalculate");
}

void __stdcall CApp::OnSheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target)
{
	log_info(helios_module, "Enter OnSheetChange");
	log_info(helios_module, "Leave OnSheetChange");
}

void __stdcall CApp::OnNewWorkbook(__lv_in _Workbook* Wb)
{
	log_info(helios_module, "Enter OnNewWorkbook");
	log_info(helios_module, "Leave OnNewWorkbook");
}

void __stdcall CApp::OnAfterCalculate()
{
	log_info(helios_module, "Enter OnAfterCalculate");
	CSumsAddin::GetAddin()->GetDoc()->OnAfterCalculate();
	log_info(helios_module, "Leave OnAfterCalculate");
}