#include "stdafx.h"
#include "CondChangedFormula.h"
#include "SumsAddin.h"
#include "CStrHash.h"

using namespace ATL;
using namespace lava::utils;

CComVariant CondChangedFormula::CacheFormula(std::string& asset_class, std::string& mode, std::string& format,
	std::vector<std::string>& fids, std::vector<double>& vals, int count)
{
	if (format.empty())
	{
		GlobalInfo config_info;
		CSumsAddin::GetAddin()->GetDoc()->GetGlobalConfigInfo(config_info);
		format = std::string("%") + config_info.m_changedSendInfo.m_formatPrecision + "f";
	}

	std::string hash_str = asset_class + mode + format;
	for (int i = 0; i < count; i++)
	{
		hash_str += fids[i];

		char val[_min_path] = { 0 };
		sprintf_s(val, _min_path, format.c_str(), vals[i]);
		hash_str += val;
	}
	uint32_t new_hash_value = CStrHash::BKDRHash(hash_str.c_str());

	CallerInfo info;
	CSumsAddin::GetAddin()->GetApp()->get_CallerInfo(&info);
	CELL_POS pos = { info.Sheet.p, info.Col, info.Row };
	bool need_collect = CachePosHashValue(pos, new_hash_value);
	if ( need_collect )
	{		
		m_single_cell = info.Caller;
		ScContribData data = { asset_class, std::pair<uint32_t, std::string>(20138, mode) };
		std::copy(fids.begin(), fids.begin() + count, std::back_inserter(data.fids));
		std::copy(vals.begin(), vals.begin() + count, std::back_inserter(data.vals));
		data.hash_val = new_hash_value;
		m_data_to_send.push_back(data);
	}
	return CComVariant(m_hash_data[new_hash_value].c_str());
}

bool CondChangedFormula::SendData(__lv_in IDispatch* Sh)
{
	int count = m_data_to_send.size();
	if (count > 1)
	{
		// Get system current time 
		SYSTEMTIME st = { 0 };
		::GetLocalTime(&st);

		// Send data to server
		bool cur_ret = CSumsAddin::GetAddin()->GetDoc()->GetCDHClient()->sendData(CDH_FUNCTION_CODE, m_data_to_send);
		wchar_t prompt[_min_path] = { 0 };
		if ( cur_ret )
		{
			wsprintf(prompt, L"%02d:%02d:%02d %4d-%02d-%02d", st.wHour, st.wMinute, st.wSecond, st.wYear, st.wMonth, st.wDay);
		}
		else
		{
			CString str;
			str.LoadString(IDS_SCCONTRIB_SEND_FAILED);
			wsprintf(prompt, L"%s", str.GetString());
		}

		for ( auto it = m_data_to_send.begin(); it != m_data_to_send.end(); it++ )
			m_hash_data[it->hash_val] = prompt;

		if ( cur_ret )
			m_data_to_send.clear();

		bool prev_sent = m_prev_sent;
		m_prev_sent = cur_ret;
		if ( prev_sent || cur_ret )
			return true;
	}
	else if (count == 1)
	{
		// Get system current time 
		SYSTEMTIME st = { 0 };
		::GetLocalTime(&st);

		// Send data to server
		bool cur_ret = CSumsAddin::GetAddin()->GetDoc()->GetCDHClient()->sendData(CDH_FUNCTION_CODE, m_data_to_send);
		wchar_t prompt[_min_path] = { 0 };
		if (cur_ret)
		{
			wsprintf(prompt, L"%02d:%02d:%02d %4d-%02d-%02d", st.wHour, st.wMinute, st.wSecond, st.wYear, st.wMonth, st.wDay);
		}
		else
		{
			CString str;
			str.LoadString(IDS_SCCONTRIB_SEND_FAILED);
			wsprintf(prompt, L"%s", str.GetString());
		}

		m_hash_data[m_data_to_send[0].hash_val] = prompt;
		if ( cur_ret )
			m_data_to_send.clear();

		bool prev_sent = m_prev_sent;
		m_prev_sent = cur_ret;
		if ( prev_sent || cur_ret )
			CSumsAddin::GetAddin()->GetApp()->RefreshSingle(m_single_cell);
	}
	return false;
}

void CondChangedFormula::SheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target)
{

}

void CondChangedFormula::ManualSend()
{
	log_info(helios_module, "force to send data where condition's upload = changed");
	m_hash_data.clear();
	m_pos_to_hash.clear();
	m_data_to_send.clear();
	CSumsAddin::GetAddin()->GetApp()->CalculateFull();
}

bool CondChangedFormula::CachePosHashValue(CELL_POS& pos, hash_value_type& new_hash_value)
{
	auto it = m_hash_data.find(new_hash_value);
	if ( it != m_hash_data.end() )
	{
		//
		// This hash value already be cached. it means this formula ever be collected.
		// 

		auto iter = m_pos_to_hash.find(pos);
		if (iter == m_pos_to_hash.end())
		{
			// 
			// This cell first be collected, but its formula ever be collected
			// so it should have the same hash value
			// for example: same formula exists in different cells
			//
			m_pos_to_hash[pos] = new_hash_value;
			return false;
		}
		else
		{
			// 
			// This cell ever be collected, that means it has formula, if its formula
			// hasn't changed, nothing to do. otherwise something need to be done: 
			//		1. use new hash value to update 
			//		2. delete it if old hash value has no longer been used
			//

			hash_value_type old_hash_value = iter->second;
			if (old_hash_value == new_hash_value)
				return false;

			m_pos_to_hash[pos] = new_hash_value;
			for (auto val : m_pos_to_hash)
				if (val.second == old_hash_value)
					return false;

			m_hash_data.erase(old_hash_value);
			return false;
		}
	}
	else
	{
		//
		// This hash value has never been cached, that means this is new formula
		//

		CString str;
		str.LoadString(IDS_SCCONTRIB_SEND_DATA);

		auto iter = m_pos_to_hash.find(pos);
		if (iter == m_pos_to_hash.end())
		{
			//
			// This cell first be collected.
			//		1. give prompt like "send data..." for this hash value
			//		2. map position to hash value.
			//
			m_hash_data[new_hash_value] = str.GetString();
			m_pos_to_hash[pos] = new_hash_value;
			return true;
		}

		// 
		// This cell ever be collected, that means it has an old formula,
		// two things need to be done:
		//		1. give prompt like "send data..." and use new hash value to update 
		//		2. delete it if old hash value has no longer been used
		//

		m_hash_data[new_hash_value] = str.GetString();
		m_pos_to_hash[pos] = new_hash_value;

		hash_value_type old_hash_value = iter->second;
		for (auto val : m_pos_to_hash)
			if (val.second == old_hash_value)
				return true;

		// old hash value has no longer been used
		m_hash_data.erase(old_hash_value);
		return true;
	}
}