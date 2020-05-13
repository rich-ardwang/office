#include "stdafx.h"
#include "CondFreqFormula.h"
#include "SumsAddin.h"
#include "CStrHash.h"

using namespace ATL;
using namespace lava::utils;

CondFreqFormula* CondFreqFormula::s_pThis = NULL;

ATL::CComVariant CondFreqFormula::CacheFormula(std::string& asset_class, std::string& mode, uint32_t freq,
	std::vector<std::string>& fids, std::vector<double>& vals, int count)
{
	if ( freq == 0 )	// comes from global config
	{
		GlobalInfo config_info;
		CSumsAddin::GetAddin()->GetDoc()->GetGlobalConfigInfo(config_info);
		freq = config_info.m_FreqSendInfo.m_defaultSendTimeSpan;
	}

	//
	// 1. Join the string base on formula.
	// 2. Calculate new hash value base on joined string.
	//
	char freq_str[32] = {0};
	sprintf_s(freq_str, 32, "%d", freq);
	std::string hash_str = asset_class + mode + freq_str;
	for (int i = 0; i < count; i++)
	{
		hash_str += fids[i];

		char val[_min_path] = { 0 };
		sprintf_s(val, _min_path, "%.5f", vals[i]);
		hash_str += val;
	}
	uint32_t new_hash_value = CStrHash::BKDRHash(hash_str.c_str());

	// Add timer
	AddClockNode(freq);

	CallerInfo info;
	CSumsAddin::GetAddin()->GetApp()->get_CallerInfo(&info);
	CELL_POS pos = { info.Sheet.p, info.Col, info.Row };	
	bool first_time = CachePosHashValue(pos, new_hash_value);
	if (NeedCollect(freq, first_time))
	{
		// 
		// 1. Construct new item.
		// 2. add item to corresponding collection
		//
		ScContribData data = { asset_class, std::pair<uint32_t, std::string>(20138, mode) };
		std::copy(fids.begin(), fids.begin() + count, std::back_inserter(data.fids));
		std::copy(vals.begin(), vals.begin() + count, std::back_inserter(data.vals));
		data.hash_val = new_hash_value;
		CollectData(freq, new_hash_value, data);
	}
	return CComVariant(m_hash_data[new_hash_value].c_str());
}

bool CondFreqFormula::SendData(__lv_in IDispatch* Sh)
{
	// Start timers ...
	if (0 == m_timer_id)
	{
		m_timer_id = ::SetTimer(NULL, 0, 1000, (TIMERPROC)CondFreqFormula::TimerCallback);
		log_info(helios_module, "create timer, id = %d", m_timer_id);
	}

	if ( m_data_to_send.size() > 0 )
	{
		//
		// Clear reach point, because the work collecting data be finished base on it.
		//
		m_reach_point.clear();

		// Get system current time 
		SYSTEMTIME st = {0};
		::GetLocalTime(&st);

		std::vector<ScContribData> collection;
		for (auto it = m_data_to_send.begin(); it != m_data_to_send.end(); it++)
			std::copy(it->second.begin(), it->second.end(), std::back_inserter(collection));
		m_data_to_send.clear();

		//
		// Send data to CDH Server, refresh prompt. 
		// 
		bool cur_ret = CSumsAddin::GetAddin()->GetDoc()->GetCDHClient()->sendData(CDH_FUNCTION_CODE, collection);
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

		for (auto val : collection)
			m_hash_data[val.hash_val] = prompt;

		// trigger refreshing prompt.
		return true;
	}
	else
	{
		//
		// 1. Collecting data failed base on m_reach_point, delete the corresponding clock.
		// 2. Clear reach point, because the work collecting data be finished base on it.
		//
		for (auto val : m_reach_point)
			DelClockNode(val);
		m_reach_point.clear();

		//
		// if all clocks not exist, kill timer.
		//
		if ( 0 == m_clock_nodes.size() && m_timer_id )
		{
			log_info(helios_module, "kill timer, id = %d", m_timer_id);
			::KillTimer(NULL, m_timer_id);
			m_timer_id = 0;
		}
	}
	return false;
}

void CondFreqFormula::SheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target)
{

}

/*
 *	Return Value: 
 *		true:	indicates this formula never been collected, this is first time.
 *		false:	indicates this formula ever been collected, this is not first time.
 *				even though this formula exists in the other cell.
 */
bool CondFreqFormula::CachePosHashValue(CELL_POS& pos, hash_value_type& new_hash_value)
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

		// 
		// This cell ever be collected, that means it has formula, if its formula
		// hasn't changed, nothing to do. otherwise something need to be done: 
		//		1. use new hash value to update 
		//		2. delete old hash value if old hash has no longer been used
		//

		hash_value_type old_hash_value = iter->second;
		if ( old_hash_value == new_hash_value )
			return false;

		m_pos_to_hash[pos] = new_hash_value;
		for (auto val : m_pos_to_hash)
			if (val.second == old_hash_value)
				return false;

		// old hash value has no longer been used
		m_hash_data.erase(old_hash_value);
		return false;
	}
	else
	{
		//
		// This hash value has never been cached, that means this is new formula
		//

		auto iter = m_pos_to_hash.find(pos);
		if (iter == m_pos_to_hash.end())
		{
			//
			// This cell first be collected.
			// 1. cache prompt for this hash value
			// 2. map position to hash value.
			// 
			CString str;
			str.LoadString(IDS_SCCONTRIB_WAIT_SEND);
			m_hash_data[new_hash_value] = str.GetString();
			m_pos_to_hash[pos] = new_hash_value;
			return  true;
		}

		// 
		// This cell ever be collected, that means it has old formula,
		// two things need to be done:
		//		1. keep old prompt and use new hash value to update 
		//		2. delete it if old hash value has no longer been used
		//

		hash_value_type old_hash_value = iter->second;
		m_hash_data[new_hash_value] = m_hash_data[old_hash_value];
		m_pos_to_hash[pos] = new_hash_value;

		for (auto val : m_pos_to_hash)
			if (val.second == old_hash_value)
				return false;

		// old hash value has no longer been used
		m_hash_data.erase(old_hash_value);
		return false;
	}
}

bool CondFreqFormula::NeedCollect(freq_type freq, bool first_time)
{
	if ( first_time )
	{
		GlobalInfo config_info;
		CSumsAddin::GetAddin()->GetDoc()->GetGlobalConfigInfo(config_info);
		return config_info.m_FreqSendInfo.m_sendDataRightNow ;
	}

	for (auto val : m_reach_point)
		if (val == freq)
			return true;

	return false;
}

bool CondFreqFormula::CollectData(freq_type freq, hash_value_type new_hash_value, ScContribData& data)
{
	if (m_data_to_send.find(freq) == m_data_to_send.end())
		m_data_to_send[freq] = std::vector<ScContribData>();

	//
	// if this formula has already been collected. we should NOT collect it again
	// for example, the same formula exists in differnt cells
	//		
	std::vector<ScContribData>& data_collecter = m_data_to_send[freq];
	for (auto val : data_collecter)
	{
		if (val.hash_val == new_hash_value)
			return false;
	}
	data_collecter.push_back(data);
	return true;
}

void CondFreqFormula::AddClockNode(freq_type freq)
{
	for (auto val : m_clock_nodes)
		if (val.freq == freq)
			return;

	CLOCK_NODE node = { freq, freq };
	m_clock_nodes.push_back(node);
}

void CondFreqFormula::DelClockNode(freq_type freq)
{
	for (auto it = m_clock_nodes.begin(); it != m_clock_nodes.end(); ++it)
	{
		if (it->freq == freq)
		{
			m_clock_nodes.erase(it);
			return;
		}
	}
}

void CondFreqFormula::TimerCallback(HWND hWnd, UINT msg, UINT_PTR id, DWORD elapse_ms)
{
	//
	// Collect freq that reaches point.
	//
	s_pThis->m_reach_point.clear();
	for (auto it = s_pThis->m_clock_nodes.begin(); it != s_pThis->m_clock_nodes.end(); ++it)
	{
		if (0 == --(it->until))
		{
			it->until = it->freq;
			s_pThis->m_reach_point.push_back(it->freq);
		}
	}

	//
	// Freq reaching point exist.
	//
	if (s_pThis->m_reach_point.size() > 0)
	{
		// Trigger collecting formula base on m_reach_point
		CSumsAddin::GetAddin()->GetApp()->CalculateFull();
	}
}