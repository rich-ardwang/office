#include "stdafx.h"
#include "resource.h"
#include "SumsAddin.h"
#include "SafeArray.h"
#include "ScContribFormula.h"
#include "lava_conv.h"
#include "CStrHash.h"
#include <thread>
#include <algorithm>
#include <regex>

#define CDH_FUNCTION_CODE               1005

using namespace Helios;
using namespace ATL;

ScContribFormula::ScContribFormula() : m_prev_sent(true)
{

}

ScContribFormula::~ScContribFormula()
{

}

void ScContribFormula::OnCalculate()
{	
	// WARNING: here cannot call get_Caller()

	int count = m_data_to_send.size();
	if ( count > 1 )
	{
		// Send data to server
		bool cur_ret = CSumsAddin::GetAddin()->GetDoc()->GetCDHClient()->sendData(CDH_FUNCTION_CODE, m_data_to_send);

		//
		// Success: Clear data after sending.
		// Failed:  Keep data, set hash data to false.
		//
		for (auto it = m_data_to_send.begin(); it != m_data_to_send.end(); it++)
			m_hash_data[it->hash_val] = cur_ret;

		if (cur_ret)
			m_data_to_send.clear();
		
		bool prev_sent = m_prev_sent;
		m_prev_sent = cur_ret;
		if ( prev_sent || cur_ret )
			CSumsAddin::GetAddin()->GetApp()->CalculateFull();
	}
	else if (count == 1)
	{
		// Send data to server
		bool cur_ret = CSumsAddin::GetAddin()->GetDoc()->GetCDHClient()->sendData(CDH_FUNCTION_CODE, m_data_to_send);

		//
		// Success: Clear data after sending.
		// Failed:  Keep data, set hash data to false.
		//
		m_hash_data[m_data_to_send[0].hash_val] = cur_ret;
		if (cur_ret)
			m_data_to_send.clear();

		bool prev_sent = m_prev_sent;
		m_prev_sent = cur_ret;
		if ( prev_sent || cur_ret )
			CSumsAddin::GetAddin()->GetApp()->RefreshSingle(m_single_cell);
	}
}

void ScContribFormula::OnAfterCalculate()
{

}

void ScContribFormula::OnStopCalculate()
{

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

	// Standardize mode
	std::string mode = standard_mode(WCHAR_TO_UTF8_EX(params[1]->bstrVal));
	if (mode.empty())
	{
		CString str;
		str.LoadString(IDS_SCCONTRIB_MODE_ERROR);
		return CComVariant(str);
	}

	// Standardize format
	std::string format = standard_format(WCHAR_TO_UTF8_EX(params[4]->bstrVal));
	if (format.empty())
	{
		CString str;
		str.LoadString(IDS_SCCONTRIB_FORMAT_ERROR);
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
	return CacheFormula(asset_class, mode, format, fids, vals, cols);
}

CComVariant ScContribFormula::CacheFormula(std::string& asset_class, std::string& mode, std::string& format, 
	std::vector<std::string>& fids, std::vector<double>& vals, int count)
{
	std::string hash_str = asset_class + mode + format;
	for (int i = 0; i < count; i++)
	{
		hash_str += fids[i];

		char val[_min_path] = { 0 };
		sprintf_s(val, format.c_str(), vals[i]);
		hash_str += val;
	}

	uint32_t hash_val = CStrHash::BKDRHash(hash_str.c_str());
	auto it = m_hash_data.find(hash_val);
	if ( it == m_hash_data.end() )
	{
		ScContribData data = { asset_class, std::pair<uint32_t, std::string>(20138, mode) };
		std::copy(fids.begin(), fids.begin() + count, std::back_inserter(data.fids));
		std::copy(vals.begin(), vals.begin() + count, std::back_inserter(data.vals));
		data.hash_val = hash_val;
		m_data_to_send.push_back(data);

		m_hash_data[hash_val] = false; // no send.

		CallerInfo info;
		CSumsAddin::GetAddin()->GetApp()->get_CallerInfo(&info);
		m_single_cell = info.Caller;
		CELL_POS pos = { info.Col, info.Row };
		auto it = m_pos_hash.find(pos);
		if (it != m_pos_hash.end())
			m_hash_data.erase(it->second);
		m_pos_hash[pos] = hash_val;

		CString str;
		str.LoadString(IDS_SCCONTRIB_SEND_DATA);
		return CComVariant(str);
	}
	else
	{
		CString str;
		str.LoadString(it->second ? IDS_FINISHED : IDS_SCCONTRIB_SEND_FAILED);
		return CComVariant(str);
	}
}

std::string ScContribFormula::standard_mode(std::string str)
{
	if ( str.find("=") == std::string::npos )
		return std::string("");

	return /*std::string("20138=") + */str;
}

std::string ScContribFormula::standard_format(std::string str)
{
	int pos = str.find("FORMAT");
	if ( pos == std::string::npos )
		return std::string("");

	std::regex reg("[0-9]+\\.[0-9]+");
	std::smatch smat;
	if ( std::regex_search(str, smat, reg) )
		return std::string("%") + smat.str() + "f";

	reg = "[0-9]+:[0-9]+";
	if ( std::regex_search(str, smat, reg) )
	{
		std::string ret = smat.str();
		ret.replace(ret.find(':'), 1, 1, '.');
		return std::string("%") + ret + "f";
	}
	return std::string("");
}