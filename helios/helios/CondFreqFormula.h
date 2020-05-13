
#pragma once
#include "ConditionFormula.h"

class CondFreqFormula : public ConditionFormula
{
	typedef uint32_t freq_type;
	struct CLOCK_NODE
	{
		uint32_t		until;
		freq_type		freq;
		bool operator < (const CLOCK_NODE& rhs) const
		{
			return this->until < rhs.until;
		}
	};

public:
	CondFreqFormula() : m_timer_id(0) { s_pThis = this; }
	~CondFreqFormula() 
	{ 
		if (m_timer_id)
			::KillTimer(NULL, m_timer_id);
	};

	ATL::CComVariant CacheFormula(std::string& asset_class, std::string& mode, uint32_t freq,
		std::vector<std::string>& fids, std::vector<double>& vals, int count);

	bool SendData(__lv_in IDispatch* Sh);
	void SheetChange(__lv_in IDispatch* Sh, __lv_in struct Range* Target);

private:
	bool CachePosHashValue(CELL_POS& pos, hash_value_type& new_hash_value); // return true: first time be collected
	bool NeedCollect(freq_type freq, bool first_time);
	bool CollectData(freq_type freq, hash_value_type new_hash_value, ScContribData& data);
	void AddClockNode(freq_type freq);
	void DelClockNode(freq_type freq);
	static void TimerCallback(HWND hWnd, UINT msg, UINT_PTR id, DWORD elapse_ms);

private:
	lava::utils::map_lv<freq_type, std::vector<ScContribData>>	m_data_to_send;

	lava::utils::vector_lv<CLOCK_NODE>		m_clock_nodes;
	lava::utils::vector_lv<freq_type>		m_reach_point;
	lava::utils::vector_lv<ATL::CString>	m_sheet_names;
	UINT_PTR								m_timer_id;
	static CondFreqFormula*					s_pThis;
};