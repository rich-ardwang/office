
#include "stdafx.h"
#include "resource.h"
#include "SumsAddin.h"
#include "SafeArray.h"
#include "ScContribFormula.h"
#include "lava_conv.h"
#include "lava_crt.h"
#include <thread>
#include <algorithm>
#include <regex>

using namespace Helios;
using namespace ATL;
using namespace lava::utils;

void ScContribFormula::OnCalculate(__lv_in IDispatch* Sh)
{	
	// WARNING: here cannot call get_Caller()
	//std::this_thread::sleep_for(std::chrono::milliseconds(200));
	bool ret1 = m_cond_changed_formula.SendData(Sh);
	bool ret2 = m_cond_freq_formula.SendData(Sh);
	if ( ret1 || ret2 )
		CSumsAddin::GetAddin()->GetUiMgr()->PostCustomMessage(WM_REFRESH, 0, 0);
}

void ScContribFormula::OnSheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target)
{
	m_cond_changed_formula.SheetChange(Sh, Target);
	m_cond_freq_formula.SheetChange(Sh, Target);
}

void ScContribFormula::OnAfterCalculate()
{

}

void ScContribFormula::OnStopCalculate()
{

}

void ScContribFormula::OnManualSend()
{
	m_cond_changed_formula.ManualSend();
}

CComVariant ScContribFormula::Calc(long& param_count, CComVariant** params)
{
	//
	// 0: asset class
	// 1: mode, like "CNYON=CICT", "CNYTN=CICT", "CNYSN=CICT"
	// 2: field ids, type is "string"
	// 3: field values, type is "double"
	//
	if (param_count < 5 || params[0]->vt != VT_BSTR || params[1]->vt != VT_BSTR
		|| !(params[2]->vt & VT_ARRAY) || !(params[3]->vt & VT_ARRAY) || params[4]->vt != VT_BSTR)
	{
		CString str;
		str.LoadString(IDS_PARAM_ERROR);
		return CComVariant(str);
	}

	LAVA_USES_CONVERSION_EX;
	std::string asset_class = WCHAR_TO_UTF8_EX(params[0]->bstrVal);

	// Check mode
	std::string mode = check_mode(WCHAR_TO_UTF8_EX(params[1]->bstrVal));
	if (mode.empty())
	{
		CString str;
		str.LoadString(IDS_SCCONTRIB_MODE_ERROR);
		return CComVariant(str);
	}

	// Standardize condition
	CONDITION condition = {um_none, "", 0};
	bool ret = parse_condition(WCHAR_TO_UTF8_EX(params[4]->bstrVal), condition);
	if ( false == ret )
	{
		CString str;
		str.LoadString(IDS_SCCONTRIB_COND_ERROR);
		return CComVariant(str);
	}

	// Extract field ids
	long lb = 0, ub = 0;
	OleSafeArray fid_arr(params[2]->parray, params[2]->vt);
	fid_arr.GetLBound(1, &lb);
	fid_arr.GetUBound(1, &ub);
	if (ub - lb + 1 < 2)
	{
		CString str;
		str.LoadString(IDS_SCCONTRIB_FIELD_IDS_ERROR);
		return CComVariant(str);
	}

	std::vector<std::string> fids;
	for (long pos = lb; pos <= ub; pos++)
	{
		CComVariant val;
		fid_arr.GetElement(&pos, &val);
		ATLASSERT(val.vt & VT_BSTR);
		fids.push_back(WCHAR_TO_UTF8_EX(val.bstrVal));
	}

	// Extract field vals
	OleSafeArray val_arr(params[3]->parray, params[3]->vt);
	val_arr.GetLBound(1, &lb);
	val_arr.GetUBound(1, &ub);
	if (ub - lb + 1 < 2)
	{
		CString str;
		str.LoadString(IDS_SCCONTRIB_FIELD_VAL_ERROR);
		return CComVariant(str);
	}

	std::vector<double> vals;
	for (long pos = lb; pos <= ub; pos++)
	{
		CComVariant val;
		val_arr.GetElement(&pos, &val);
		if (val.vt == VT_EMPTY)
		{
			CString str;
			str.LoadString(IDS_EMPTY);
			return CComVariant(str);
		}
		vals.push_back(val.dblVal);
	}

	int cols = std::min<int>(fids.size(),vals.size());
	if (condition.m_upload_mode == um_changed)
		return m_cond_changed_formula.CacheFormula(asset_class, mode, condition.m_ft, fids, vals, cols);
	else
		return m_cond_freq_formula.CacheFormula(asset_class, mode, condition.m_fq, fids, vals, cols);
}

std::string ScContribFormula::check_mode(std::string str)
{
	if ( str.find("=") == std::string::npos )
		return std::string("");

	return /*std::string("20138=") + */str;
}

bool ScContribFormula::parse_condition(std::string str, CONDITION& ret)
{
	std::vector<std::string> pairs;
	lava::utils::split(str.c_str(), " ", pairs);
	for (auto val : pairs)
	{
		std::vector<std::string> key_value;
		lava::utils::split(val.c_str(), "=", key_value);
		if ( 2 != key_value.size() )
			return false;

		// convert characters(key and value) to lower case
		transform(key_value[0].begin(), key_value[0].end(), key_value[0].begin(), ::tolower);
		transform(key_value[1].begin(), key_value[1].end(), key_value[1].begin(), ::tolower);

		if ( 0 == key_value[0].compare("upload") )
		{
			ret.m_upload_mode = key_value[1].compare("changed") == 0 ? um_changed :
								key_value[1].compare("frequency") == 0 ? um_frequency : um_none;
			if ( ret.m_upload_mode == um_none )
				return false;
		}
		else if ( key_value[0].compare("ft") == 0 )
		{
			std::regex reg("[0-9]+\\.[0-9]+");
			std::smatch smat;
			if (std::regex_search(key_value[1], smat, reg))
				ret.m_ft = std::string("%") + smat.str() + "f";
			else
				return false;
		}
		else if ( key_value[0].compare("fq") == 0 )
		{
			std::regex reg("[0-9]+[s|m|h]");
			std::smatch smat;
			if (std::regex_search(key_value[1], smat, reg))
			{
				std::string value = smat.str();
				int count = value.size();
				int prefix = atoi(value.substr(0, count - 1).c_str());
				int suffix = value[count-1];
				ret.m_fq = (suffix == 's') ? prefix : (suffix == 'm') ? prefix * 60 : (suffix == 'h') ? prefix * 3600 : 0;
			}
			else
				return false;
		}
	}
	return true;
}